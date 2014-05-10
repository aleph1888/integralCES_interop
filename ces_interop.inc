<?php

require_once drupal_get_path('module', 'ces_bank') . '/ces_bank.logic.inc';
require_once drupal_get_path('module', 'ces_bank') . '/ces_bank.module';


/**
 *
 * INDEX Callback implementation for 
 * 		ces_interop.module//ces_interop_gateway_services_resources()
 *
 * @params string $type See ces_interop_gateway_access() description.
 * @params string $id The identificator of the fetched entity (if any)
 *
 * @returns array wether....
 *
 *		$type == 'client' 	[ (string)context ]
 *		$type == 'logged' 	[ (int)id, (string)name, int(amount) ]
 *		$type == 'user' 	[ (int)id, (string)name, int(amount) ]
 *		$type == 'payment' 	[ (int)id, (int)buyer, (int)seller, (int)(amount), (string)concept, (int)state ]
 *
 * NOTE: (int)state is a valid TransactionInterface::STATE
 */
function _ces_interop_gateway_index( $type, $id = 0 ) {

	switch( $type ) {

		case "client":
			$context = _ces_interop_gateway_get_oauth_context();
			return  $context;
			break;

		case "logged":
			$user_id =  _ces_interop_gateway_get_oauth_user();
           		$account = ces_account_load( (int)$user_id );
                        if ( $account ) {
                                return array(
                                        "id" => $account["id"],
                                        "name" => $account["name"],
                                        "balance" => $account["balance"]
                                );
                        } else {
                                return NULL;
                        }
			break;

		case "user":
			$bank = new bank();

			$account = $bank->getAccountByName( $id );

			if ( $account ) {
				return array(
					"id" => $account["id"],
					"name" => $account["name"],
					"balance" => $account["balance"]
				);
			} else {
				return NULL;
			}
			break;

		case "payment":
			$bank = new bank();

			$transaction = ces_transaction_load( (int)$id );

			if ( $transaction ) {
				return array(
					"id" =>  $transaction["id"],
					"buyer" => $bank->getTransactionToAccount( $transaction )["id"],
					"seller" => $bank->getTransactionFromAccount( $transaction )["id"],
					"amount" => $transaction["amount"],
					"concept" => $transaction["concept"],
					"state" => $transaction["state"]
				);
			} else {
				return NULL;
			}
			break;
	}
}


/**
 *
 * CREATE Callback implementation for 
 * 		ces_interop.module//ces_interop_gateway_services_resources()
 *
 * @params array $params
 * 		string $buyer Buyer account name.
 * 		string $seller Buyer account name.
 * 		int $amount Amount in cents.
 * 		string $concept Any description for transaction.
 *
 * @returns array [ (int)id, (bool)result, (int)state ]
 *
 * NOTE: (int)state is a valid  TransactionInterface::STATE
 */
function _ces_interop_gateway_create( $params ) {

	$bank = new bank();

	// Second gatekeeper (First is on _ces_interop_gateway_access())
	// there, only check is logged user. Here we check if
	// seller is the logged user.
	$logged_user_id =  _ces_interop_gateway_get_oauth_user();
	$buyer_user = $bank->getAccountByName( $params["buyer"] );
	$seller_user = $bank->getAccountByName( $params["seller"] );
	if ( $buyer_user ) 
		$buyer_user_id = $buyer_user["id"];
	else 
		$buyer_user_id = "Not found: " .  $params["buyer"];
	
	if ( $logged_user_id != $buyer_user_id ) {
		$error = sprintf( "Buyer: %s| Logged: %s", $buyer_user_id, $logged_user_id );
                return array(
                        "id" => 0,
                        "result" => "This user is no allowed for this transaction." . $error,
                        "state" => 11
                );
	}

error_log("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww");
error_log($seller["id"]);

	// Set transaction info
	$record = array(
		"fromaccount" => $buyer_user["id"],
		"toaccount" => $seller_user["id"],
		"amount" => $params["amount"],
		"concept" => $params["concept"],
		"id" => 0
	);

	// create & apply
	// Why is not working try/catch, consumer gets Exception object parsed to string????
	try {

		$bank->createTransaction( $record );
		$result = $bank->applyTransaction( $record['id'] );
		$transaction = ces_transaction_load( $record['id'] );

	} catch ( Exception $e ) {

		$error = $e->getMessage();
	}

	// retrieve and return
	if ( $record['id'] > 0 ) {
		return array( 
			"id" => $record['id'], 
			"result" => $record['id'] > 0, 
			"state" => $transaction["state"]
		); 
	} else {
		return array( 
			"id" => 0, 
			"result" => $error, 
			"state" => 11
		);
	}

}


/**
 *
 * UPDATE Callback implementation for 
 * 		ces_interop.module//ces_interop_gateway_services_resources()
 *
 * @params int $id A Transaction identificator.
 *
 * @returns bool $result
  */
function _ces_interop_gateway_update( $params ) {

	$bank = new bank();

	$result = $bank->applyTransaction( $params['id'] );

	return $result;

}


/**
 *
 * Uses request token to determine related context.
 *
 * @return string The name of the context
 */
function _ces_interop_gateway_get_oauth_context() {

        $req = DrupalOAuthRequest::from_request();
        $consumer_key = $req->get_parameter( 'oauth_consumer_key' );
        $consumer = DrupalOAuthConsumer::loadProviderByKey( $consumer_key );

	$context = oauth_common_context_load($consumer->context);
	if ( $context )
		return $consumer->context;
	else
		return null;

}


/**
 * 
 * Uses request token to determinate logged user
 *
 *
 * @return int User id
 */
function _ces_interop_gateway_get_oauth_user() {

	$req = DrupalOAuthRequest::from_request();
	$consumer_key = $req->get_parameter( 'oauth_consumer_key' );
	$consumer = DrupalOAuthConsumer::loadProviderByKey( $consumer_key );
	$token_key = $req->get_parameter( 'oauth_token' );
	$token = DrupalOAuthToken::loadbyKey( $token_key, $consumer );
	return $token->uid;

}


/**
 *
 * Resource gatekeeper.
 *
 * @params string $op A value in {"interop gateway payment", "interop gateway index" }
 *	
 * @params array $args Depends on "$op"
 *
 *	$op == "interop gateway index"
 *
 *		Requires $args[0] A value in { 'client', 'logged', 'user', 'transaction' }
 *
 *			"client" 	-> TRUE if a existing a valid consumer_key related to a context in request. TODO: Check CES Interop exacdt context! 
 *					--> Meaning: Site client ask for a valid key/secret site configuration.
 *
 *			"logged"	-> TRUE if an existing logged in user found inside request token.
 *					--> Meaning: A user authorization has been performed so exists a valid access_token.
 *
 *			 "user"   	-> Same as client
 *					--> Meaning: Client wants to retrieve a user.
 *
 *			"payment"	-> Same as client.
 *					--> Meaning: Client wants to retrieve a payment.
 *	
 */
function _ces_interop_gateway_access( $op, $args = NULL ) {

	$ACCESS = FALSE;

	switch( $op ) {

		case "interop gateway create":		
                        $user_id =  _ces_interop_gateway_get_oauth_user();
                        $ACCESS = $user_id > 0;
                        break;

		case "interop gateway index":
			
			switch( $args[0] ) {
				case "client" || "user":
                                        $context =  _ces_interop_gateway_get_oauth_context();
                                        $ACCESS = $context;
					break;
				case "logged":
					$user_id =  _ces_interop_gateway_get_oauth_user();
		                        $ACCESS = $user_id > 0;
					break;
				case "payment":
		                        $user_id =  _ces_interop_gateway_get_oauth_user();
                                        $ACCESS = $user_id > 0;
					break;
				case "user":
                                        $user_id =  _ces_interop_gateway_get_oauth_user();
                                        $ACCESS = $user_id > 0;
                                        break;
			}
			break;
	}
	return $ACCESS;

}

?>

