BASIC CART PLUS
------------------------

This Backdrop CMS module is working in MVP mode but not secure yet.  Please do not use for handling credit cards.
I am making an attempt to finish this in 2015.  Thank you!

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

Basic cart is a very simple shopping cart / checkout process for Backdrop, that just sends 2 emails after each order. Along with the shopping cart,
this module also provides a block where you can view your cart's content.

You can sell existing nodes as physical products, nodes as donation forms, nodes as events (or virtual products), roles as memberships, roles as subscriptions through a built in
Paypal, Braintree or Stripe payment processors.  Both Javascript (IFrame) and PHP payment forms are available through admin checkboxes.  You can also choose to forego the processor -- for example, "pay in person when you pick it up or attend".

Backdrop's taxonomy system is your product categories, and Backdrop's Views are your reporting system.  It will also save all order/customer information to the payment processor because they have better reporting/admin experience than building our own.

The basic idea is that you want to sell a small number of products and just want a install-the-module-and-process-cards experience.
To support more options... you probably should not use this module or fork it and make it your own.  Drupal Commerce, Ebay, Magento, Shopify might be for you!

Features

* The possibility of choosing the content types that can be added to the cart or marked for "insider content".
* The possibility of sending, or not, an email to the customer once an order is placed.
* Custom email messages for both the site administrator and the customer, along with the order details.
* A block with the contents of your shopping cart.
* Order details can be saved in two places: in a receipt node, and to the processor

TESTED
-------

Working, but not working yet securely yet.

KNOWN ISSUES
---------------------

need to incorporate Stripe Iframe solution securely.
need to save all Braintree and Paypal account/order information on checkout

SPECIAL THANKS
--------------

- biolithic for the vision and completion
- dicix <https://www.drupal.org/u/dicix>
- Jen Lampton <https://www.drupal.org/u/jenlampton>

REQUIREMENTS
------------

elements
form validation scripts
libraries
existing nodes/content types
Paypal, Braintree or Stripe merchant accounts and PHP SDK's if you so choose to use them

INSTALLATION
------------

Install this module using the official Backdrop CMS instructions at https://backdropcms.org/guide/modules

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

installs a membership role for you to use


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
