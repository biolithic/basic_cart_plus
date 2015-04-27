BASIC CART BRAINTREE
===================

CONTENTS OF THIS FILE
---------------------

 - Introduction
 - Tested
 - Known Issues
 - Special Thanks
 - Requirements
 - Installation
 - Coming From Drupal?
 - Usage
 - License
 - Credits
 - Maintainers

INTRODUCTION
------------

Basic cart is a very simple shopping cart / checkout process for Backdrop,
that just sends 2 emails after each order. Along with the shopping cart,
this module also provides a block where you can view your cart's content.

This is the Basic Cart e-commerce module with Braintree Payments credit card processing built into the admin/checkout process.
The basic idea is that you want to sell a small number of products and just want a install-the-module-and-process-cards experience.
To support multiple payment types, etc... you probably should not use this module or fork it and make it your own.

It's ideal for small websites with only a few products or other content types
being sold, for example touristic offers.  It is much simpler to get started for simple or few products.

Features

* The possibility of choosing the content types that can be added to the cart.
* The possibility of sending, or not, an email to the customer once an order is placed.
* Custom email messages for both the site administrator and the customer, along with the order details.
* A block with the contents of your shopping cart.

TESTED
-----

Working on Backdrop 1.0. Will resolve to iron out bugs -- probably beta stage.

KNOWN ISSUES
---------------------

@todo

SPECIAL THANKS
--------------

- dicix <https://www.drupal.org/u/dicix>
- Jen Lampton <https://www.drupal.org/u/jenlampton>


REQUIREMENTS
------------

none

INSTALLATION
------------

Install this module using the official Backdrop CMS instructions at https://backdropcms.org/guide/modules

Because the libraries module/concept is not yet set or taught, the actual Braintree code library is packaged with this module until a 3rd-party code library is agreed upon in Backdrop.

COMING FROM DRUPAL?
-------------------

Main difference from Ubercart and Commerce

The main difference from Ubercart and Commerce is the possibility of choosing
the content types that can be added to the cart. It also doesn't bother you
with the Product SKU, that can be a pain when you have just a few products.

Basic Cart for Drupal stored node objects in the user $_SESSION variable and used pre-made Views, Features, etc... The Backdrop port failed with this error because of it: Fatal error: Call to undefined function module_implements() in /Applications/MAMP/htdocs/backdrop/core/includes/bootstrap.inc on line 3467 This happens when you add a product to the cart.

Basic Cart for Backdrop stores node id's as strings in the user $_SESSION variable and no pre-made etc...

PERMISSIONS
------------

@todo


USAGE
-----

Install the module.

If the goods you want to sell don't exist on your website, create a new content type for them. If you already have a content type for them you can skip this step.

Select the content type/s on the module configuration page: Configuration - Basic Cart or admin/config/basic_cart/settings.

Create a view for the content type/s to get the shop's overview. If you already have it, you can skip this step.

You have a block with the cart preview. Activate it ;-)

Start to add you're products in the newly created content type or in the one you already have.


LICENSE
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for complete text.

CREDITS
-----------

This module is based on the Basic Cart module for Drupal, originally written and maintained by a large number of contributors, including:

- dicix <https://www.drupal.org/u/dicix>

MAINTAINERS
-----------

 - biolithic <https://github.com/biolithic>

Ported to Backdrop by:

 - biolithic <https://github.com/biolithic>
