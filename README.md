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
