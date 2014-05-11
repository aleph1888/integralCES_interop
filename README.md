integralCES_interop
===================

Implements gateway interop for http://integralces.net

Features
----------------
1) Hooking [*services_resources*](http://drupalcontrib.org/api/drupal/contributions!services!services.services.api.php/function/hook_services_resources/6) in ces_interop.module

2) Consuming entrypoint/resource:

* http://yourCES_Server/gateway/interop

3) Available methods:

* GET, with params:
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
 
* POST (creates new payment), with params:
  - array **params**
  
    		string **buyer** Buyer account name.
  			string **seller** Buyer account name.
  			int **amount** Amount in cents.
  			string **concept** Any description for transaction.

Info
------------------

Testing development [server](http://cicicdev.enredaos.net/cesinterop)

Drupal CES [issue](https://drupal.org/node/2215167)

API
-------------
There is an existin [API](https://github.com/aleph1888/integralCES_consumer/tree/master/includes/icesSDKv0) and a (use it)[API](https://github.com/aleph1888/integralCES_consumer) example.


Doc
---------------
[Documentation](https://wiki.enredaos.net/index.php?title=COOPFUND-DEV#integralCES_interop)

[Developer](http://www.integralces.net/doc/developer)

Contribute
--------------
BTC @ 1DNxbBeExzv7JvXgL6Up5BSUvuY4gE8q4A
