<?php

require_once drupal_get_path('module', 'ces_bank') . '/ces_bank.logic.inc';
require_once drupal_get_path('module', 'ces_bank') . '/ces_bank.module';


/**
 *
 * @params int $id User identifier
 *
 * @return array [ 	(int)user_id, 
 *			(string)user_name, 
 *			(array)accounts	
 */
function _ces_interop_get_user( $id ) {

	$user = user_load( $id );

	if ( $user ) {

		// user data
	        $return_user["user_id"] = $id;
        	$return_user["user_name"] = user_load( $id )->name;

		$bank = new bank();
		$return_user["accounts"] =  $bank->getUserAccounts( $id );
		return $return_user;
	}

	return NULL;

}


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
 *		$type == 'logged' 	[ (int)id, (string)name, (array of arrays[ (int)id, (string)name, (int)balance) ] accounts ]
 *		$type == 'user' 	[ (int)id, (string)name, (array of arrays[ (int)id, (string)name, (int)balance) ] accounts ]
 *		$type == 'account' 	[ (int)id, (string)name, (int)balance) ]
 *		$type == 'payment' 	[ (int)id, (int)buyer_account, (int)seller_account, (int)amount, (string)concept, (int)state ]
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

			$logged_user_id =  _ces_interop_gateway_get_oauth_user();
                        if ( $logged_user_id > 0 ) 
				return _ces_interop_get_user( $logged_user_id  );
	                
                        return NULL;                        
			break;

		case "user":

			if ( (int)$id > 0  )
                                return _ces_interop_get_user( (int)$id );

                        return NULL;
                        break;

		case "account":
			$bank = new bank();
			if ( (string)$id ) 
				return $bank->getAccountByName( (string)$id );

			return NULL;
			break;

		case "payment":
			$bank = new bank();

			$transaction = ces_transaction_load( (int)$id );

			if ( $transaction ) {
				return array(
					"id" =>  $transaction["id"],
					"buyer_account" => $bank->getTransactionToAccount( $transaction )["id"],
					"seller_account" => $bank->getTransactionFromAccount( $transaction )["id"],
					"amount" => $transaction["amount"],
					"concept" => $transaction["concept"],
					"state" => $transaction["state"]
				);
			}
			return NULL;
			break;

	}
}


/**
 *
 * CREATE Callback implementation for 
 * 		ces_interop.module//ces_interop_gateway_services_resources()
 *
 * @params array $params
 * 		string $buyer_account Buyer account name.
 * 		string $seller_account Buyer account name.
 * 		int $amount Amount in cents.
 * 		string $concept Any description for transaction.
 *
 * @returns array [ (int)id, (bool)result, (int)state ]
 *
 * NOTE: (int)state is a valid  TransactionInterface::STATE
 */
function _ces_interop_gateway_create( $params ) {

	$bank = new bank();

	$buyer_account = $bank->getAccountByName( $params["buyer_account"] );
	$seller_account = $bank->getAccountByName( $params["seller_account"] );

	// Does accounts exists?
	$missing_account = "Account named: %s does not exists. ";
	
	$error = "";
	if ( ! $buyer_user )
		$error = sprintf( $missing_acount, $buyer_account );

        if ( ! $seller_user )
                $error .= sprintf( $missing_acount, $seller_account );

       	if ( $error ) {
                return array(
                        "id" => 0,
                        "result" => $error,
                        "state" => 11
                );
        }

	// Is logged user buyer user?
	$logged_user_id =  _ces_interop_gateway_get_oauth_user();

	// get logged in accounts
	$logged_accounts = $bank->getUserAccounts( $logged_user_id );

	// check if this accounts it is related to logged in user
	foreach ( $logged_accounts as $account) {
		$is_owned_by_buyer = ( $account["id"] == $buyer_account["id"] );
		if ( $is_owned_by_buyer )
			break;
	}

       	if ( ! $is_owned_by_buyer ) {
                $error = sprintf( "Buyer: %s| Logged: %s", $params["buyer_account"], $logged_accounts["name"] );
                return array(
                        "id" => 0,
                        "result" => "This user is no allowed for this transaction: " . $error,
                        "state" => 11
                );
        }

	// Set transaction info
	$record = array(
		"fromaccount" => $buyer_account["id"],
		"toaccount" => $seller_account["id"],
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
	
	if ( $consumer ) {
		$context = oauth_common_context_load($consumer->context);
		if ( $context )
			return $consumer->context;
	}
	
	return NULL;

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
	
	if ( $consumer ) {
		$token_key = $req->get_parameter( 'oauth_token' );
		$token = DrupalOAuthToken::loadbyKey( $token_key, $consumer );
		return $token->uid;
	}
	return 0;

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
 *                       "account"      -> Same as client
 *                                      --> Meaning: Client wants to retrieve a user.
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
				case "client" || "user" || "account" :
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

