<?php

require_once drupal_get_path('module', 'ces_bank') . '/ces_bank.logic.inc';
require_once drupal_get_path('module', 'ces_bank') . '/ces_bank.module';

/**
 *
 * INDEX Callback implementation for 
 * 		ces_interop.module//ces_interop_gateway_services_resources()
 *
 * @params string CONST $type Any value in {'user', 'type'}
 * @params int $id Depending on $type, an Account or a Transaction identificator.
 *
 * @returns array 
 *		$type == 'user' [ (int)id, (string)name, int(amount) ]
 *		$type == 'payment' [ (int)id, (int)buyer, (int)seller, (int)(amount), (string)concept, (int)state ]
 * NOTE: (int)state is a valid  TransactionInterface::STATE
 */
function _ces_interop_gateway_index ( $type, $id ) {

	switch( $type ) {

		case "user":
			$bank = new bank();

			$account = $bank->ces_account_load( (int)$id );

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

			$transaction = $bank->ces_transaction_load( (int)$id );

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
 * 		int $buyer Buyer account id.
 * 		int $seller Buyer account id.
 * 		int $amount Amount in cents.
 * 		string $concept Any description for transaction.
 *
 * @returns array [ (int)id, (bool)result, (int)state ]
 *
 * NOTE: (int)state is a valid  TransactionInterface::STATE
 */
function _ces_interop_gateway_create( $params ) {

	$bank = new bank();

	$record = array(
		"fromaccount" => $params["buyer"],
		"toaccount" => $params["seller"],
		"amount" => $params["amount"],
		"concept" => $params["concept"],
		"id" => 0
	);

	$bank->createTransaction( $record );

	//$result = $bank->applyTransaction( $record['id'] );
	$transaction = $bank->ces_load_transaction( $record['id'] );

	if ( $record['id'] > 0 ) {
		return array( 
			"id" => $record['id'], 
			"result" => $result, 
			"state" => $transaction["state"] 
	) else {
		return array( 
			"id" => 0, 
			"result" => FALSE, 
			"state" => 0
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


function _ces_interop_gateway_access( $op ) {
	error_log($op);
	return true;
}

?>

