integralCES_interop
===================

Implements gateway interop for http://integralces.net

Features
----------------
1) Hooking [*services_resources*](http://drupalcontrib.org/api/drupal/contributions!services!services.services.api.php/function/hook_services_resources/6) in ces_interop.module

Available methods:
 *      GET index: Retrieves json object representing a user or a payment.
 *      POST create: Creates a new transaction.
 *      PUT update: Applies a transaction.

2) Implementing callbacks in ces_interop.inc

Info
------------------

Testing development [server](http://cicicdev.enredaos.net/cesinterop)

Drupal CES [issues](https://drupal.org/project/issues/1367140)

API
-------------
There is an existin [API](https://github.com/aleph1888/integralCES_consumer/tree/master/includes/icesSDKv0) and a (use it)[[API](https://github.com/aleph1888/integralCES_consumer) example.


Doc
---------------
[Documentation](https://wiki.enredaos.net/index.php?title=COOPFUND-DEV#integralCES_interop)

[Developer](http://www.integralces.net/doc/developer)

Contribute
--------------
BTC @ 1DNxbBeExzv7JvXgL6Up5BSUvuY4gE8q4A
