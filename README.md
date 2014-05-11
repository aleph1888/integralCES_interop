integralCES_interop
===================

Implements payment gateway interop for http://integralces.net

Whats this for
----------------------------------
[A web service for a payment gateway](https://wiki.enredaos.net/images/2/21/IntegralcesGeneric.png), that implements [OAuth 1.0](https://wiki.enredaos.net/index.php?title=File:Oauth-drupal.jpg) in [3-legged](https://wiki.enredaos.net/images/thumb/c/c2/OAuth_Process.jpg/800px-OAuth_Process.jpg) way by hooking [*services_resources*](http://drupalcontrib.org/api/drupal/contributions!services!services.services.api.php/function/hook_services_resources/6) in ces_interop.module


Server configuracion
----------------------------------
Create web service by using your drupal UI Admin interface. Here you have the [steps](https://github.com/aleph1888/integralCES_interop/blob/master/SERVICES_SERVER_CONFIG.md).


Using info
----------------

1) Http consuming: 

A common service can be consumed by http://yourCES_Server/entrypoint/resource, if you have configured your REST server named as it is named on [tutorial](https://github.com/aleph1888/integralCES_interop/blob/master/SERVICES_SERVER_CONFIG.md) *gateway*, then your url will be:

* http://yourCES_Server/gateway/interop

2) Available methods:

2.1) GET, with params:

IN:
  - string **type** { 'client', 'logged', 'user', 'paymen' }
 
  			"client" 	-> TRUE if a existing a valid consumer_key related to a context in request. TODO: Check CES Interop exacdt context! 
  					--> Meaning: Site client ask for a valid key/secret site configuration.
  
  			"logged"	-> TRUE if an existing logged in user found inside request token.
  					--> Meaning: A user authorization has been performed so exists a valid access_token.
  
  			 "user"   	-> Same as client
  					--> Meaning: Client wants to retrieve a user.
  
  			"payment"	-> Same as client.
  					--> Meaning: Client wants to retrieve a payment.

 - string **id** Identifies the retrieved entity

OUT:
  - array wether....
 
 		$type == 'client' 	[ (string)context ]
 		$type == 'logged' 	[ (int)id, (string)name, int(amount) ]
 		$type == 'user' 	  [ (int)id, (string)name, int(amount) ]
 		$type == 'payment' 	[ (int)id, (int)buyer, (int)seller, (int)(amount), (string)concept, (int)state ]
 		
2.2) POST (creates new payment), with params:

IN:
  - array **params**
  
    		string **buyer** Buyer account name.
  			string **seller** Buyer account name.
  			int **amount** Amount in cents.
  			string **concept** Any description for transaction.

OUT:
 - array 
        [ (int)id, (bool)result, (int)state ]
 
NOTE: (int)state is a valid  TransactionInterface::STATE

Info
------------------

Testing development [server](http://cicicdev.enredaos.net/cesinterop).

Drupal CES [issue](https://drupal.org/node/2215167).

API
-------------
There is an existing API [icesSDKv0](https://github.com/aleph1888/integralCES_consumer/tree/master/includes/icesSDKv0) and a [use it](https://github.com/aleph1888/integralCES_consumer) example.

Testing development [server](http://cicicdev.enredaos.net/integralCES_consumer/index.php).


Doc
---------------
[Documentation](https://wiki.enredaos.net/index.php?title=COOPFUND-DEV#integralCES_interop).

[Developer](http://www.integralces.net/doc/developer).

Contribute
--------------
BTC @ 1DNxbBeExzv7JvXgL6Up5BSUvuY4gE8q4A
