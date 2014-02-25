<?php
/**
 * @file
 * basic_cart_braintree_order.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function basic_cart_braintree_order_user_default_permissions() {
  $permissions = array();

  // Exported permission: administer basic cart braintree.
  $permissions['administer basic cart braintree'] = array(
    'name' => 'administer basic cart braintree',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'basic_cart_braintree',
  );

  // Exported permission: use basic cart braintree.
  $permissions['use basic cart braintree'] = array(
    'name' => 'use basic cart braintree',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
    ),
    'module' => 'basic_cart_braintree',
  );

  // Exported permission: view basic cart braintree orders.
  $permissions['view basic cart braintree orders'] = array(
    'name' => 'view basic cart braintree orders',
    'roles' => array(
      0 => 'administrator',
      1 => 'authenticated user',
    ),
    'module' => 'basic_cart_braintree',
  );

  return $permissions;
}
