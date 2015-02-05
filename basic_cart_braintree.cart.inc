<?php

/**
 * @file
 * Basic cart shopping cart implementation functions.
 */

/**
 * Callback function for cart listing.
 */
function basic_cart_braintree_cart() {
  $cart = basic_cart_braintree_get_cart();

  if (empty($cart)) {
    return t('Your cart is empty.');
  }
  return backdrop_get_form('basic_cart_braintree_cart_form');
}

/**
 * Shopping cart form.
 */
function basic_cart_braintree_cart_form() {
  // Getting the shopping cart.
  $cart = basic_cart_braintree_get_cart();
  // And now the form.
  $form['cartcontents'] = array(
    // Make the returned array come back in tree form.
    '#tree' => TRUE,
    '#prefix' => '<div class="basic-cart-cart basic-cart-grid">',
    '#suffix' => '</div>',
  );
  // Cart elements.
  foreach ($cart as $nid => $node) {
    $form['cartcontents'][$nid] = array(
      '#type' => 'textfield',
      '#size' => 1,
      '#default_value' => $node->basic_cart_braintree_quantity,
      '#theme' => 'basic_cart_braintree_render_cart_element',
    );
  }
  // Total price.
  $form['total_price'] = array(
    '#markup' => t('Total price'),
    '#prefix' => '<div class="basic-cart-cart basic-cart-grid">',
    '#suffix' => '</div>',
    '#theme' => 'basic_cart_braintree_cart_total_price',
  );
  // Buttons.
  $form['buttons'] = array(
    // Make the returned array come back in tree form.
    '#tree' => TRUE,
    '#prefix' => '<div class="row"><div class="basic-cart-call-to-action cell">',
    '#suffix' => '</div></div>',
  );
  // Update button.
  $form['buttons']['update'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  // Checkout button.
  $form['buttons']['checkout'] = array(
    '#type' => 'submit',
    '#value' => t('Checkout'),
  );

  return $form;
}

/**
 * Shopping cart form.
 */
function basic_cart_braintree_cart_form_submit($form_id, $form_state) {
  foreach ($form_state['values']['cartcontents'] as $nid => $value) {
    $quantity = (int) $value;
    if ($quantity > 0) {
      $_SESSION['basic_cart_braintree']['cart'][$nid]->basic_cart_braintree_quantity = $quantity;
    }
    // If the quantity is zero, we just remove the node from the cart.
    elseif ($quantity == 0) {
      unset($_SESSION['basic_cart_braintree']['cart'][$nid]);
    }
  }

  if ($form_state['values']['op'] == t('Checkout')) {
    backdrop_goto('checkout');
  }
  else {
    backdrop_set_message(t('Shopping cart updated.'));
  }
}

/**
 * Callback function for cart/add/.
 *
 * @param int $nid
 *   We are using the node id to store the node in the shopping cart
 */
function basic_cart_braintree_add_to_cart($nid = NULL) {
  $nid = (int) $nid;
  if ($nid > 0) {
    // If a node is added more times, just update the quantity.
    $cart = basic_cart_braintree_get_cart();
    if (!empty($cart) && in_array($nid, array_keys($cart))) {
      // Clicked 2 times on add to cart button. Increment quantity.
      $_SESSION['basic_cart_braintree']['cart'][$nid]->basic_cart_braintree_quantity++;
    }
    else {
      $node = node_load($nid);
      $node->basic_cart_braintree_quantity = 1;
      // Adding description.
      $body = field_get_items('node', $node, 'body');
      $description = isset($body[0]['value']) ? check_plain(strip_tags($body[0]['value'])) : '';
      $node->basic_cart_braintree_node_description = $description;
      // Unit price.
      $unit_price = field_get_items('node', $node, 'price');
      $unit_price = isset($unit_price[0]['value']) ? check_plain(strip_tags($unit_price[0]['value'])) : 0;
      $node->basic_cart_braintree_unit_price = $unit_price;

      // Storing in session.
      $_SESSION['basic_cart_braintree']['cart'][$nid] = $node;
    }
  }
  backdrop_set_message(t('Shopping cart updated.'));
  $redirect = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_redirect_user_after_add_to_cart');
  if (empty($redirect)) {
    backdrop_goto('cart');
  }
  elseif ($redirect == '<none>') {
    $referer = $_SERVER["HTTP_REFERER"];
    backdrop_goto($referer);
  }
  else {
    backdrop_goto($redirect);
  }
}

/**
 * Callback function for cart/remove/.
 *
 * @param int $nid
 *   We are using the node id to remove the node in the shopping cart
 */
function basic_cart_braintree_remove_from_cart($nid = NULL) {
  $nid = (int) $nid;
  if ($nid > 0) {
    unset($_SESSION['basic_cart_braintree']['cart'][$nid]);
  }
  backdrop_set_message(t('Shopping cart updated.'));
  backdrop_goto('cart');
}

/**
 * Function for shopping cart retrieval.
 *
 * @param int $nid
 *   We are using the node id to store the node in the shopping cart
 *
 * @return mixed
 *   Returning the shopping cart contents.
 *   An empty array if there is nothing in the cart
 */
function basic_cart_braintree_get_cart($nid = NULL) {
  if (isset($nid)) {
    return $_SESSION['basic_cart_braintree']['cart'][$nid];
  }
  if (isset($_SESSION['basic_cart_braintree']['cart'])) {
    return $_SESSION['basic_cart_braintree']['cart'];
  }
  // Empty cart.
  return array();
}

/**
 * Returns the final price for the shopping cart.
 *
 * @return mixed $total_price
 *   The total price for the shopping cart.
 */
function basic_cart_braintree_get_total_price() {
  // Building the return array.
  $return = array(
    'price' => 0,
    'vat' => 0,
    'total' => 0,
  );
  $cart = basic_cart_braintree_get_cart();
  if (empty($cart)) {
    return (object) $return;
  }

  $total_price = 0;
  foreach ($cart as $nid => $node) {
    if (isset($node->basic_cart_braintree_quantity) && isset($node->basic_cart_braintree_unit_price)) {
      $total_price += $node->basic_cart_braintree_quantity * $node->basic_cart_braintree_unit_price;
    }
  }

  $return['price'] = $total_price;

  // Checking whether to apply the VAT or not.

  $vat_is_enabled = (int) config_get('basic_cart_braintree.settings', 'basic_cart_braintree_vat_state');
  if (!empty ($vat_is_enabled) && $vat_is_enabled) {
    $vat_value = (float) config_get('basic_cart_braintree.settings', 'basic_cart_braintree_vat_value');
    $vat_value = ($total_price * $vat_value) / 100;
    $total_price += $vat_value;
    // Adding VAT and total price to the return array.
    $return['vat'] = $vat_value;
  }

  $return['total'] = $total_price;
  return (object) $return;
}

/**
 * Shopping cart reset.
 */
function basic_cart_braintree_empty_cart() {
  unset($_SESSION['basic_cart_braintree']['cart']);
}

/**
 * Formats the input $price in the desired format.
 *
 * @param float $price
 *   The price in the raw format.
 * @return $price
 *   The price in the custom format.
 */
function basic_cart_braintree_price_format($price) {
  $format = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_price_format');
  $currency = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_currency');
  $price = (float) $price;
  switch ($format) {
    case 0:
      $price = number_format($price, 2, ',', ' ') . ' ' . $currency;
      break;

    case 1:
      $price = number_format($price, 2, '.', ' ') . ' ' . $currency;
      break;

    case 2:
      $price = number_format($price, 2, '.', ',') . ' ' . $currency;
      break;

    case 3:
      $price = number_format($price, 2, ',', '.') . ' ' . $currency;
      break;

    case 4:
      $price = $currency . ' ' . number_format($price, 2, ',', ' ');
      break;

    case 5:
      $price = $currency . ' ' . number_format($price, 2, '.', ' ');
      break;

    case 6:
      $price = $currency . ' ' . number_format($price, 2, '.', ',');
      break;

    case 7:
      $price = $currency . ' ' . number_format($price, 2, ',', '.');
      break;

    default:
      $price = number_format($price, 2, ',', ' ') . ' ' . $currency;
      break;
  }
  return $price;
}

/**
 * Returns the available price formats.
 *
 * @return $formats
 *   A list with the available price formats.
 */
function _basic_cart_braintree_price_format() {
  $currency = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_currency');
  return array(
    0 => t('1 234,00 @currency', array('@currency' => $currency)),
    1 => t('1 234.00 @currency', array('@currency' => $currency)),
    2 => t('1,234.00 @currency', array('@currency' => $currency)),
    3 => t('1.234,00 @currency', array('@currency' => $currency)),

    4 => t('@currency 1 234,00', array('@currency' => $currency)),
    5 => t('@currency 1 234.00', array('@currency' => $currency)),
    6 => t('@currency 1,234.00', array('@currency' => $currency)),
    7 => t('@currency 1.234,00', array('@currency' => $currency)),
  );
}

/**
 * Checkout.
 */

/**
 * Checkout form implementation.
 */
function basic_cart_braintree_checkout() {
  $shopping_cart = basic_cart_braintree_get_cart();
  // Price.
  $price = basic_cart_braintree_get_total_price();
  $total = basic_cart_braintree_price_format($price->total);

  $options = array(
    'cart' => $shopping_cart,
    'price' => $total,
  );

  // Checking the VAT.
  $vat_is_enabled = (int) config_get('basic_cart_braintree.settings', 'basic_cart_braintree_vat_state');
  if (!empty ($vat_is_enabled) && $vat_is_enabled) {
    $options['vat'] = basic_cart_braintree_price_format($price->vat);
  }

  // The flat cart (just the listing part).
  $cart = theme('basic_cart_braintree_cart_flat', $options);

  // If the cart is empty, we don't want to show the checkout form.
  if (empty($shopping_cart)) {
    return $cart;
  }

  $form = backdrop_get_form('basic_cart_braintree_create_form');
  $form = backdrop_render($form);
  return $cart . $form;
}

/**
 * Checkout form.
 */
function basic_cart_braintree_checkout_form() {
  $form['basic_cart_braintree_checkout_name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#description' => t('Please enter your name.'),
  );

  $form['basic_cart_braintree_checkout_email'] = array(
    '#title' => t('Email'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#description' => t('Please enter your email.'),
  );

  $form['basic_cart_braintree_checkout_phone'] = array(
    '#title' => t('Phone'),
    '#type' => 'textfield',
    '#description' => t('Please enter your phone.'),
  );

  $form['basic_cart_braintree_checkout_address'] = array(
    '#title' => t('Address'),
    '#type' => 'textfield',
    '#description' => t('Please enter your address.'),
  );

  $form['basic_cart_braintree_checkout_city'] = array(
    '#title' => t('City'),
    '#type' => 'textfield',
    '#size' => 40,
    '#description' => t('Please enter your city.'),
  );

  $form['basic_cart_braintree_checkout_zipcode'] = array(
    '#title' => t('Zip code'),
    '#type' => 'textfield',
    '#size' => 16,
    '#description' => t('Please enter your zipcode.'),
  );

  $form['basic_cart_braintree_checkout_message'] = array(
    '#title' => t('Message'),
    '#type' => 'textarea',
    '#description' => t('If you wish to add a comment, please use this message area.'),
  );

  $form['basic_cart_braintree_checkout_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit order'),
  );

  return $form;
}

/**
 * Checkout form validation.
 */
function basic_cart_braintree_checkout_form_validate($form, &$form_state) {
  if (!valid_email_address($form_state['values']['basic_cart_braintree_checkout_email'])) {
    form_set_error('basic_cart_braintree_checkout_email', t('Please enter a valid email address.'));
  }
}

/**
 * Checkout form submit proccess.
 * Register order and send emails.
 */
function basic_cart_braintree_checkout_form_submit($form, &$form_state) {
  // %ORDER_DETAILS% placeholder.
  $order_details = '';
  $cart = basic_cart_braintree_get_cart();
  $price = basic_cart_braintree_get_total_price();
  $total_price = basic_cart_braintree_price_format($price->total);

  $vat_is_enabled = (int) config_get('basic_cart_braintree.settings', 'basic_cart_braintree_vat_state');
  if (!empty ($vat_is_enabled) && $vat_is_enabled) {
    $vat = basic_cart_braintree_price_format($price->vat);
  }

  // Registering the new order to the database.
  if (module_exists('basic_cart_braintree_order')) {
    $name = $form_state['values']['basic_cart_braintree_checkout_name'];
    $email = $form_state['values']['basic_cart_braintree_checkout_email'];
    $order_details_ = array(
      'phone' => $form_state['values']['basic_cart_braintree_checkout_phone'],
      'city' => $form_state['values']['basic_cart_braintree_checkout_city'],
      'zipcode' => $form_state['values']['basic_cart_braintree_checkout_zipcode'],
      'address' => $form_state['values']['basic_cart_braintree_checkout_address'],
      'message' => $form_state['values']['basic_cart_braintree_checkout_message'],
    );
    $order = basic_cart_braintree_order_register_order($name, $email, $order_details_);

    // Order successfully placed. Custom hook for other modules.
    foreach (module_implements('basic_cart_braintree_order') as $module) {
      $function = $module . '_basic_cart_braintree_order';
      // Will call all modules implementing hook_basic_cart_braintree_order() with the order node as argument.
      $function($order);
    }
  }

  // Building the order details.
  $i = 0;
  foreach ($cart as $nid => $node) {
    $unit_price = basic_cart_braintree_price_format($node->basic_cart_braintree_unit_price);
    $order_details .= ++$i . '. ' . $node->title . "\t" .
                      $node->basic_cart_braintree_quantity . ' x ' . $unit_price . "\n";
  }
  $order_details .= "\n";
  $order_details .= "Total price: " . $total_price;
  $vat_is_enabled = (int) config_get('basic_cart_braintree.settings', 'basic_cart_braintree_vat_state');
  if ($vat_is_enabled) {
    $order_details .= "\n";
    $order_details .= "Total VAT: " . $vat;
  }
  // Pleaceholder replacement.
  $search = array(
    '%CUSTOMER_NAME', '%CUSTOMER_EMAIL', '%CUSTOMER_PHONE', '%CUSTOMER_CITY',
    '%CUSTOMER_ZIPCODE', '%CUSTOMER_ADDRESS', '%CUSTOMER_MESSAGE', '%ORDER_DETAILS',
  );
  $replace = array(
    $form_state['values']['basic_cart_braintree_checkout_name'], $form_state['values']['basic_cart_braintree_checkout_email'],
    $form_state['values']['basic_cart_braintree_checkout_phone'], $form_state['values']['basic_cart_braintree_checkout_city'],
    $form_state['values']['basic_cart_braintree_checkout_zipcode'], $form_state['values']['basic_cart_braintree_checkout_address'],
    $form_state['values']['basic_cart_braintree_checkout_message'], $order_details,
  );
  // Admin message.
  $message_html = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_admin_message');
  $message_html = str_replace($search, $replace, $message_html);

  // Admin mail.
  $params['admin_message'] = $message_html;
  $site_mail = config_get('system.core', 'site_mail');

  $admin_emails = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_admin_emails');
  if (empty ($admin_emails)) {
    // Sending mail to admin.
    $message = backdrop_mail('basic_cart_braintree', 'admin_mail', $site_mail, language_default(), $params);

    $mails_sent = 0;
    if ($message['result']) {
      $mails_sent++;
    }
  }
  else {
    $admin_emails = explode("\n", $admin_emails);
    if (is_array($admin_emails) && !empty($admin_emails)) {
      $ok = FALSE;
      foreach ($admin_emails as $admin_email) {
        // Sending mail to each admin.
        $message = backdrop_mail('basic_cart_braintree', 'admin_mail', $admin_email, language_default(), $params);

        // Verifing that the mail was sent for at least one email address.
        if ($message['result']) {
          $ok = TRUE;
        }
      }

      $mails_sent = 0;
      if ($ok) {
        $mails_sent++;
      }
    }
  }


  // User email.
  $send_user_mail = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_send_user_message');
  if ($send_user_mail) {
    $message_html = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_user_message');
    $message_html = str_replace($search, $replace, $message_html);
    $params['user_message'] = $message_html;

    // Sending mail.
    $message = backdrop_mail('basic_cart_braintree', 'user_mail', $form_state['values']['basic_cart_braintree_checkout_email'], language_default(), $params);

    if ($message['result']) {
      $mails_sent++;
    }
  }

  if ($mails_sent >= 1) {
    basic_cart_braintree_empty_cart();
    backdrop_goto('checkout/thank-you');
  }
  else {
    backdrop_set_message(t('There was a problem in submitting your order. Please try again later.'), 'error');
  }
}


/**
 * Implements hook_mail().
 */
function basic_cart_braintree_mail($key, &$message, $params) {
  switch ($key) {
    case 'admin_mail':
      $message['subject'] = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_admin_subject');
      $message['body'][] = filter_xss($params['admin_message']);
      break;

    case 'user_mail':
      $message['subject'] = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_user_subject');
      $message['body'][] = filter_xss($params['user_message']);
      break;
  }
}

/**
 * Callback for thank you page.
 */
function basic_cart_braintree_checkout_thank_you() {
  $title = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_thank_you_title');
  backdrop_set_title($title);
  $message = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_thank_you_message');
  return filter_xss($message);
}















/**
 * First page of a multi-page form.
 * This page provides billing fields.
 *
 * @param array $form
 *   Define form elements.
 *
 * @param array $form_state
 *   State of current forms including passed values.
 *
 * @return array
 */
function basic_cart_braintree_create_form($form, $form_state)
{

    // Return payment section of form if page_num == 2.
    if (!empty($form_state['page_num']) && $form_state['page_num'] == 2) {
        return basic_cart_braintree_form_payment($form, $form_state);
    }

    // Set page_num to 1 and build page 1.
    $form_state['page_num'] = 1;
    $form                   = array();
    // Define form as hierarchy so we can access both pages as trees.
    $form['#tree']          = TRUE;


    $form['billing'] = array(
        '#type' => 'fieldset',
        '#title' => t('Billing & Account Details - Enter your account for this site, or this will create one for you.'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE
    );

    // Check for $form_state values returned on form rebuild and when submitting the back button.
    global $user;
    if ($user->uid == 0) {
        $email                    = isset($form_state['values']['billing']['email']) ? $form_state['values']['billing']['email'] : '';
        $form['billing']['email'] = array(
            '#type' => 'textfield',
            '#title' => t('Email'),
            '#required' => TRUE,
            '#default_value' => $email,

            // Perform ajax validation.
            '#ajax' => array(
                'callback' => '_basic_cart_braintree_form_email_ajax_validate',
                'wrapper' => 'email-error',
                'effect' => 'slide'
            ),
            '#suffix' => '<div id="email-error" style="color:#ff0000;"> </div>'
        );

        $form['billing']['password'] = array(
            '#type' => 'password',
            '#title' => t('Password'),
            '#required' => TRUE
        );

        $form['billing']['conf-password'] = array(
            '#type' => 'password',
            '#title' => t('Confirm Password'),
            '#required' => TRUE
        );
    } else {
        $email                    = $user->mail;
        $form['billing']['email'] = array(
            '#type' => 'textfield',
            '#title' => t('Email'),
            '#required' => TRUE,
            '#default_value' => $email,

            // Perform ajax validation.
            '#ajax' => array(
                'callback' => '_basic_cart_braintree_form_email_ajax_validate',
                'wrapper' => 'email-error',
                'effect' => 'slide'
            ),
            '#suffix' => '<div id="email-error" style="color:#ff0000;"> </div>'
        );
    }

    $testvariable = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_firstname');
    if ($testvariable > 0) {
        $firstname                    = isset($form_state['values']['billing']['firstname']) ? $form_state['values']['billing']['firstname'] : '';
        $form['billing']['firstname'] = array(
            '#type' => 'textfield',
            '#title' => t('First Name'),
            '#required' => TRUE,
            '#default_value' => $firstname
        );
    }

    $testvariable = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_lastname');
    if ($testvariable > 0) {
        $lastname                    = isset($form_state['values']['billing']['lastname']) ? $form_state['values']['billing']['lastname'] : '';
        $form['billing']['lastname'] = array(
            '#type' => 'textfield',
            '#title' => t('Last Name'),
            '#required' => TRUE,
            '#default_value' => $lastname
        );
    }

    $testvariable = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_phone');
    if ($testvariable > 0) {
        $phone                    = isset($form_state['values']['billing']['phone']) ? $form_state['values']['billing']['phone'] : '';
        $form['billing']['phone'] = array(
            '#type' => 'textfield',
            '#title' => t('Phone'),
            '#required' => TRUE,
            '#default_value' => $phone
        );
    }

    $testvariable = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_address');
    if ($testvariable > 0) {
        $address                    = isset($form_state['values']['billing']['address']) ? $form_state['values']['billing']['address'] : '';
        $form['billing']['address'] = array(
            '#type' => 'textfield',
            '#title' => t('Address Line 1'),
            '#required' => TRUE,
            '#default_value' => $address
        );
    }

    $testvariable = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_address2');
    if ($testvariable > 0) {
        $address2                    = isset($form_state['values']['billing']['address2']) ? $form_state['values']['billing']['address2'] : '';
        $form['billing']['address2'] = array(
            '#type' => 'textfield',
            '#title' => t('Address Line 2'),
            '#default_value' => $address2
        );
    }

    $testvariable = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_city');
    if ($testvariable > 0) {
        $city                    = isset($form_state['values']['billing']['city']) ? $form_state['values']['billing']['city'] : '';
        $form['billing']['city'] = array(
            '#type' => 'textfield',
            '#title' => t('City'),
            '#required' => TRUE,
            '#default_value' => $city
        );
    }

    $testvariable = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_county');
    if ($testvariable > 0) {
        $county                    = isset($form_state['values']['billing']['county']) ? $form_state['values']['billing']['county'] : '';
        $form['billing']['county'] = array(
            '#type' => 'textfield',
            '#title' => t('County'),
            '#required' => TRUE,
            '#default_value' => $county
        );
    }

    $testvariable = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_state');
    if ($testvariable > 0) {
        $state                    = isset($form_state['values']['billing']['state']) ? $form_state['values']['billing']['state'] : '';
        $form['billing']['state'] = array(
            '#type' => 'textfield',
            '#title' => t('State'),
            '#required' => TRUE,
            '#default_value' => $state
        );
    }

    $testvariable = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_zip');
    if ($testvariable > 0) {
        $zip                    = isset($form_state['values']['billing']['zip']) ? $form_state['values']['billing']['zip'] : '';
        $form['billing']['zip'] = array(
            '#type' => 'textfield',
            '#title' => t('Zip'),
            '#required' => TRUE,
            '#size' => 10,
            '#maxlength' => 10,
            '#default_value' => $zip
        );
    }

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Next',
        '#submit' => array(
            'basic_cart_braintree_form_tracker'
        ),
        '#validate' => array(
            'basic_cart_braintree_form_billing_validate'
        )
    );
    return $form;
}

/**
 * Track what page the form is on and rebuild form.
 *
 * @param array $form
 *   Elements to make up the form.
 *
 * @param array $form_state
 *   Form element value state of current form.
 */
function basic_cart_braintree_form_tracker($form, &$form_state)
{
    $form_state['page_values'][1] = $form_state['values'];

    if (!empty($form_state['page_values'][2])) {
        $form_state['values'] = $form_state['page_values'][2];
    }

    // Define page number on form rebuild.
    $form_state['page_num'] = 2;
    $form_state['rebuild']  = TRUE;
}

/**
 * Set page_num to 1 and rebuild form when back button is clicked.
 *
 * @param array $form
 *   Form elements to build the form.
 *
 * @param array $form_state
 *   Form state elements to pass values.
 */
function basic_cart_braintree_form_back($form, &$form_state)
{
    $form_state['values']   = $form_state['page_values'][1];
    $form_state['page_num'] = 1;
    $form_state['rebuild']  = TRUE;
}

/**
 * Provide page two of form to collect payment information and process with basic_cart_braintree.
 *
 * @param array $form
 *   Form elements to build the form.
 *
 * @param array $form_state
 *   Form state elements to pass values.
 *
 * @return array
 *   return form and form_state arrays.
 */
function basic_cart_braintree_form_payment($form, $form_state)
{
    backdrop_add_js('https://js.braintreegateway.com/v1/braintree.js', 'external');
    backdrop_add_js(backdrop_get_path('module', 'basic_cart_braintree') . '/js/basic_cart_braintree.js');

    $form['payment'] = array(
        '#type' => 'fieldset',
        '#title' => t('Payment Details'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE
    );

    $form['payment']['instructions'] = array(
        '#type' => 'markup',
        '#title' => t('Demo information'),
        '#markup' => '<p>Please use the demo credit card # of 4242424242424242 and CVC # of 123 to fake buy this.</p>'
    );

    $form['payment']['braintreetoken'] = array(
        '#type' => 'hidden',
        '#title' => t('braintreetoken')
    );

    $form['payment']['card'] = array(
        '#type' => 'textfield',
        '#title' => t('Card'),
        '#size' => 20,
        '#maxlength' => 16,
        '#description' => t('Enter your card number'),
        '#attributes' => array(
            'class' => array(
                'card-number'
            ),
            'data-encrypted-name' => 'number',
            'autocomplete' => 'off'
        )
        // '#required' => TRUE,
    );
    $form['payment']['cvc']  = array(
        '#type' => 'textfield',
        '#title' => t('CVC'),
        '#size' => 3,
        '#maxlength' => 3,
        '#description' => t('Enter the CVC number printed on the back of your card.'),
        '#attributes' => array(
            'class' => array(
                'card-cvc'
            ),
            'data-encrypted-name' => 'cvv',
            'autocomplete' => 'off'
        )
        //'#required' => TRUE,
    );

    $month_range              = range('01', '12');
    $month_options            = array_combine($month_range, $month_range);
    $form['payment']['month'] = array(
        '#type' => 'select',
        '#title' => t('Month'),
        '#options' => $month_options,
        '#attributes' => array(
            'class' => array(
                'card-expire-month'
            )
        )
    );

    $year_range              = range(date('Y'), date('Y') + 5);
    $year_options            = array_combine($year_range, $year_range);
    $form['payment']['year'] = array(
        '#type' => 'select',
        '#title' => t('Year'),
        '#options' => $year_options,
        '#attributes' => array(
            'class' => array(
                'card-expire-year'
            )
        ),
        '#suffix' => '<span class="payment-errors" style="color:#ff0000;"> </span>'
    );

    $form['html'] = array(
        '#type' => 'markup',
        '#markup' => '<a href="https://www.braintreegateway.com/merchants/n3cdm2sgkjqt5c4g/verified" target="_blank"><img src="https://braintree-badges.s3.amazonaws.com/05.png" border="0"/></a><br>For more information, see the <a href="https://www.braintreepayments.com/developers/security">Security Docs</a><br />'
    );

$one = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_publishable_testkey');
$two = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_publishable_livekey');
$three = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_gonelive');

    if ($three > 0) {
        $publishable_api_key = $two;
    } else {
        $publishable_api_key = $one;
    }

    $form['basic_cart_braintree_publishable_key'] = array(
        '#type' => 'hidden',
        '#value' => $publishable_api_key,
        '#attributes' => array(
            'id' => array(
                'basic_cart_braintree_publishable_key'
            )
        )
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
        '#submit' => array(
            'basic_cart_braintree_form_callback'
        ),
        '#attributes' => array(
            'id' => array(
                'braintreesubmit'
            )
        )
    );

    $form['back'] = array(
        '#type' => 'submit',
        '#value' => t('<< Back'),
        '#submit' => array(
            'basic_cart_braintree_form_back'
        ),
        '#limit_validation_errors' => array()
    );

$one = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_publishable_testkey');
$two = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_publishable_livekey');
$three = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_gonelive');
    if ($three > 0) {
        $basic_cart_braintree_publishable_key = $two;
    } else {
        $basic_cart_braintree_publishable_key = $one;
    }

$one = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_secret_testkey');
$two = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_secret_livekey');
$three = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_gonelive');

    if ($three > 0) {
        $basic_cart_braintree_secret_key = $two;
    } else {
        $basic_cart_braintree_secret_key = $one;
    }
    return $form;
}

/**
 * Ajax validation on email and password.
 *
 * @param array $form
 *   Elements to build the form.
 *
 * @param array $form_state
 *   Values passed during form submission.
 *
 * @return string
 */
function _basic_cart_braintree_form_email_ajax_validate($form, &$form_state)
{
    // Validate as a true email format.
    $email  = check_plain(filter_xss($form_state['values']['billing']['email']));
    $query  = 'SELECT name FROM {users} where mail = :mail';
    $result = db_query($query, array(
        ':mail' => $email
    ))->fetchField();

    if (!empty($result)) {
        $error = '<div id="email-error" style="color:#ff0000;">User is already registered.</div>';
        return $error;
    }

    // Validate as a true email format.`
    elseif (filter_var($form_state['values']['billing']['email'], FILTER_VALIDATE_EMAIL) == FALSE) {
        $error = '<div id="email-error" style="color:#ff0000;">The email is not valid.</div>';
        return $error;
    } else {
        $noerror = '<div id="email-error"> </div>';
        return $noerror;
    }
}



/**
 * This is a validation form that checks if the user is already signed up.
 */
function basic_cart_braintree_check_customer($email, $password)
{
    $email    = check_plain(filter_xss($email));
    $password = check_plain(filter_xss($password));
    $query    = 'SELECT name FROM {users} where mail = :mail';
    $result   = db_query($query, array(
        ':mail' => $email
    ))->fetchField();

    // If no errors and the backdrop account was not found create a new user.
    if (empty($result)) {
        // Create User
$array                 = explode("@", $email);
$customername          = $array[0];
$user = entity_create('user', array ());
$user->name = $customername;
$user->pass = $password;
$user->mail = $email;
$user->status = 1;
$user->access = 0;
$user->login = 0;
$user->timezone = config_get('system.core', 'date_default_timezone');
$user->data = FALSE;
$user->save();

$user_obj          = user_load_by_name($customername);
$form_state        = array();
$form_state['uid'] = $user_obj->uid;
user_login_submit(array(), $form_state);
// user_login_finalize();
}
    return $result;
}

/**
 * Validate billing section of form (page 1).
 *
 * @param array $form
 *   Elements to build the form.
 *
 * @param array $form_state
 *   Values passes during form submission.
 */
function basic_cart_braintree_form_billing_validate($form, $form_state)
{
    // Validate as a true email format.
    if (filter_var($form_state['values']['billing']['email'], FILTER_VALIDATE_EMAIL) == FALSE) {
        form_set_error('billing][email', t('This email is not valid.'));
    }
    $email  = check_plain(filter_xss($form_state['values']['billing']['email']));
    $query  = 'SELECT name FROM {users} where mail = :mail';
    $result = db_query($query, array(
        ':mail' => $email
    ))->fetchField();

    global $user;
    if ($user->uid == 0) {
        if ($form_state['values']['billing']['password'] != $form_state['values']['billing']['conf-password']) {
            form_set_error('billing][conf-password', t('The passwords did not match.'));
        }
        if (!empty($result)) {
            backdrop_set_message("The user email is already registered.  Please login to continue.");
            backdrop_goto("user/login");
        }
    }
}

/**
 * Process Braintree Charge and save user data.
 *
 * @param array $form
 * @param array $form_state
 */
function basic_cart_braintree_form_callback($form, $form_state)
{

    $one = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_publishable_testkey');
    $two = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_publishable_livekey');
    $three = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_gonelive');

    if ($three > 0) {
        $basic_cart_braintree_publishable_key = $two;
    } else {
        $basic_cart_braintree_publishable_key = $one;
    }

    $one = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_secret_testkey');
    $two = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_secret_livekey');
    $three = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_gonelive');

    if ($three > 0) {
        $basic_cart_braintree_secret_key = $two;
    } else {
        $basic_cart_braintree_secret_key = $one;
    }

    $one = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_merchantId_testkey');
    $two = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_merchantId_livekey');
    $three = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_gonelive');

    if ($three > 0) {
        $basic_cart_braintree_merchantId = $two;
    } else {
        $basic_cart_braintree_merchantId = $one;
    }

    //  empty($basic_cart_braintree_secret_key) ? NULL : basic_cart_braintree::setApiKey($basic_cart_braintree_secret_key);
    Braintree_Configuration::environment(config_get('basic_cart_braintree.settings', 'basic_cart_braintree_environment'));
    Braintree_Configuration::merchantId($basic_cart_braintree_merchantId);
    Braintree_Configuration::publicKey($basic_cart_braintree_publishable_key);
    Braintree_Configuration::privateKey($basic_cart_braintree_secret_key);

    // Assign billing data for easier referencing.
    $billing_values = $form_state['page_values']['1'];
    $price          = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_price') * 100;
    global $user;
    if ($user->uid == 0) {
        $customer_id = basic_cart_braintree_check_customer($billing_values['billing']['email'], $billing_values['billing']['password']);
    }

  $order_details = '';
  $cart = basic_cart_braintree_get_cart();
  $price = basic_cart_braintree_get_total_price();
  $total_price = basic_cart_braintree_price_format($price->total);

  $vat_is_enabled = (int) config_get('basic_cart_braintree.settings', 'basic_cart_braintree_vat_state');
  if (!empty ($vat_is_enabled) && $vat_is_enabled) {
    $vat = basic_cart_braintree_price_format($price->vat);
  }

  // Building the order details.
  $i = 0;
  foreach ($cart as $nid => $node) {
    $unit_price = basic_cart_braintree_price_format($node->basic_cart_braintree_unit_price);
    $order_details .= ++$i . '. ' . $node->title . "\t" .
                      $node->basic_cart_braintree_quantity . ' x ' . $unit_price . "\n";
  }
  $order_details .= "\n";
  $order_details .= "Total price: " . $total_price;
  $vat_is_enabled = (int) config_get('basic_cart_braintree.settings', 'basic_cart_braintree_vat_state');
  if ($vat_is_enabled) {
    $order_details .= "\n";
    $order_details .= "Total VAT: " . $vat;
  }
  $total_price = $total_price * 1;
  backdrop_set_message($total_price);

    $result = Braintree_Transaction::sale(array(
        'amount' => $total_price,
        'creditCard' => array(
            'number' => $form_state['values']['card'],
            'expirationMonth' => $form_state['values']['month'],
            'expirationYear' => $form_state['values']['year']
        )
    ));

    if ($result->success) {

        if (!empty($billing_values['billing']['firstname'])) {
            $contactname = $billing_values['billing']['firstname'];
            $firstName   = $billing_values['billing']['firstname'];
        } else {
            $contactname = "unknown";
            $firstName   = 'unknown';
        }

        if (!empty($billing_values['billing']['lastname'])) {
            $lastName = $billing_values['billing']['lastname'];
        } else {
            $lastName = 'unknown';
        }

        if (!empty($billing_values['billing']['email'])) {
            $email = $billing_values['billing']['email'];
        } else {
            $email = 'unknown';
        }

        if (!empty($billing_values['billing']['phone'])) {
            $phone = $billing_values['billing']['phone'];
        } else {
            $phone = 'unknown';
        }

        if (!empty($billing_values['billing']['address'])) {
            $streetAddress = $billing_values['billing']['address'];
        } else {
            $streetAddress = 'unknown';
        }

        if (!empty($billing_values['billing']['address2'])) {
            $extendedAddress = $billing_values['billing']['address2'];
        } else {
            $extendedAddress = 'unknown';
        }

        if (!empty($billing_values['billing']['address2'])) {
            $extendedAddress = $billing_values['billing']['address2'];
        } else {
            $extendedAddress = 'unknown';
        }

        if (!empty($billing_values['billing']['city'])) {
            $locality = $billing_values['billing']['city'];
        } else {
            $locality = 'unknown';
        }

        if (!empty($billing_values['billing']['state'])) {
            $region = $billing_values['billing']['state'];
        } else {
            $region = 'unknown';
        }

        if (!empty($billing_values['billing']['zip'])) {
            $postalCode = $billing_values['billing']['zip'];
        } else {
            $postalCode = 'unknown';
        }

        $result2 = Braintree_Customer::create(array(
            'firstName' => $firstName,
            'lastName' => $lastName,
            'email' => $email,
            'phone' => $phone,
            'creditCard' => array(
                'number' => $form_state['values']['card'],
                'expirationMonth' => $form_state['values']['month'],
                'expirationYear' => $form_state['values']['year'],
                'cardholderName' => 'Mike Jones',
                'billingAddress' => array(
                    'firstName' => $firstName,
                    'lastName' => $lastName,
                    'streetAddress' => $streetAddress,
                    'extendedAddress' => $extendedAddress,
                    'locality' => $locality,
                    'region' => $region,
                    'postalCode' => $postalCode,
                    'countryCodeAlpha2' => 'US'
                )
            )
        ));

        $customer   = $result2->customer;
        $creditCard = $customer->creditCards[0];
        backdrop_set_message(t((" Thank you for your order! Transaction success ID: " . check_plain($result->transaction->id))));

  // Registering the new order to the database.
  if (module_exists('basic_cart_braintree_order')) {
    $name = $billing_values['billing']['firstname'] . " " . $billing_values['billing']['lastname'];
    $email = $billing_values['billing']['email'];


    $order_details_ = array(
    'phone' => (empty($billing_values['billing']['phone'])) ? 'default' : $billing_values['billing']['phone'],
    'city' => (empty($billing_values['billing']['city'])) ? 'default' : $billing_values['billing']['city'],
    'zipcode' => (empty($billing_values['billing']['zip'])) ? 'default' : $billing_values['billing']['zip'],
    'address' => (empty($billing_values['billing']['address'])) ? 'default' : $billing_values['billing']['address'],
    'message' => (empty($billing_values['billing']['message'])) ? 'default' : $billing_values['billing']['message'],
    );
    $order = basic_cart_braintree_order_register_order($name, $email, $order_details_);

    // Order successfully placed. Custom hook for other modules.
    foreach (module_implements('basic_cart_braintree_order') as $module) {
      $function = $module . '_basic_cart_braintree_order';
      // Will call all modules implementing hook_basic_cart_braintree_order() with the order node as argument.
      $function($order);
    }
  }

  // Pleaceholder replacement.
  $search = array(
    '%CUSTOMER_NAME', '%CUSTOMER_EMAIL', '%CUSTOMER_PHONE', '%CUSTOMER_CITY',
    '%CUSTOMER_ZIPCODE', '%CUSTOMER_ADDRESS', '%CUSTOMER_MESSAGE', '%ORDER_DETAILS',
  );

  $name = $billing_values['billing']['firstname'] . " " . $billing_values['billing']['lastname'];
  $replace = array(
    $name, empty($billing_values['billing']['email']) ? 'default' : $billing_values['billing']['email'],
    empty($billing_values['billing']['phone']) ? 'default' : $billing_values['billing']['phone'], empty($billing_values['billing']['city']) ? 'default' : $billing_values['billing']['city'],
    empty($billing_values['billing']['zip']) ? 'default' : $billing_values['billing']['zip'], empty($billing_values['billing']['address']) ? 'default' : $billing_values['billing']['address'],
    empty($billing_values['billing']['message']) ? 'default' : $billing_values['billing']['message'], $order_details,
  );
  // Admin message.
  $message_html = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_admin_message');
  $message_html = str_replace($search, $replace, $message_html);

  // Admin mail.
  $params['admin_message'] = $message_html;
  $site_mail = config_get('system.core', 'site_mail');
  $admin_emails = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_admin_emails');
  if (empty ($admin_emails)) {
    // Sending mail to admin.
    $message = backdrop_mail('basic_cart_braintree', 'admin_mail', $site_mail, language_default(), $params);

    $mails_sent = 0;
    if ($message['result']) {
      $mails_sent++;
    }
  }
  else {
    $admin_emails = explode("\n", $admin_emails);
    if (is_array($admin_emails) && !empty($admin_emails)) {
      $ok = FALSE;
      foreach ($admin_emails as $admin_email) {
        // Sending mail to each admin.
        $message = backdrop_mail('basic_cart_braintree', 'admin_mail', $admin_email, language_default(), $params);

        // Verifing that the mail was sent for at least one email address.
        if ($message['result']) {
          $ok = TRUE;
        }
      }

      $mails_sent = 0;
      if ($ok) {
        $mails_sent++;
      }
    }
  }


  // User email.
  $send_user_mail = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_send_user_message');
  if ($send_user_mail) {
    $message_html = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_user_message');
    $message_html = str_replace($search, $replace, $message_html);
    $params['user_message'] = $message_html;

    // Sending mail.
    $message = backdrop_mail('basic_cart_braintree', 'user_mail', $billing_values['billing']['email'], language_default(), $params);

    if ($message['result']) {
      $mails_sent++;
    }
  }

  if ($mails_sent >= 1) {
    basic_cart_braintree_empty_cart();

  }
  else {
    backdrop_set_message(t('There was a problem in submitting your order. Please try again later.'), 'error');
  }




        global $user;
        $uid = $user->uid; // User ID of user that you want to add role to.
        if ($role = user_role_load_by_name("paywall_member")) {
            user_multiple_role_edit(array(
                $uid
            ), 'add_role', $role->rid);
        }

        $testvariable = config_get('basic_cart_braintree.settings', 'basic_cart_braintree_sendemail');
        if ($testvariable > 0) {
            $module = 'basic_cart_braintree';
            $key    = 'key';
            $to     = $billing_values['billing']['email'];

            $language                 = language_default();
            $params                   = array();
            $from                     = NULL;
            $send                     = FALSE;
            $message                  = backdrop_mail($module, $key, $to, $language, $params, $from, $send);
            $sitename                 = config_get('system.core', 'site_name');
            $siteadmin                = config_get('system.core', 'site_mail');
            $subject                  = 'Receipt for order from ' . $sitename;
            $message['headers']['CC'] = $siteadmin;
            $message['subject']       = $subject;
            $message['body']          = array();
            $message['body'][]        = $to . ", login to " . $_SERVER['HTTP_HOST'] . $_SERVER['REQUEST_URI'] . " to see your order for $" . config_get('paywallbraintree.settings', 'basic_cart_braintree_price') . " on " . date('l jS \of F Y h:i:s A');

            // Retrieve the responsible implementation for this message.
            $system = backdrop_mail_system($module, $key);

            // Format the message body.
            $message = $system->format($message);

            // Send e-mail.
            $message['result'] = $system->mail($message);

            if ($message['result'] == TRUE) {
                backdrop_set_message(t('Your receipt has been sent. Thank you!'));
            } else {
                backdrop_set_message(t('There was a problem sending your receipt and it was not sent.'), 'error');
            }
        }
        backdrop_goto('checkout/thank-you');

    }

    elseif ($result->transaction) {
        backdrop_set_message(t("Error processing transaction:"), 'error');
        backdrop_set_message(t("\n  message: " . check_plain($result->message)), 'error');
        backdrop_set_message(t("\n  code: " . check_plain($result->transaction->processorResponseCode)), 'error');
        backdrop_set_message(t("\n  text: " . check_plain($result->transaction->processorResponseText)), 'error');
    } else {
        $errormessages = serialize($result->errors->deepAll());
        backdrop_set_message(t("Message: " . check_plain($result->message)), 'error');
        backdrop_set_message(t("\nValidation errors: \n"), 'error');
        backdrop_set_message(t($errormessages), 'error');
    }
}
