<?php
/**
 * @file
 * Basic Cart Plus shopping cart implementation functions.
 */
/**
 * Callback function for cart listing.
 */
function basic_cart_plus_cart()
{
    $cart = basic_cart_plus_get_cart();
    if (empty($cart)) {
        return t('Your cart is empty.');
    }
    return backdrop_get_form('basic_cart_plus_cart_form');
}
/**
 * Shopping cart form.
 */
function basic_cart_plus_cart_form()
{
    // Getting the shopping cart.
    $cart                 = basic_cart_plus_get_cart();
    // And now the form.
    $form['cartcontents'] = array(
        // Make the returned array come back in tree form.
        '#tree' => TRUE,
        '#prefix' => '<div class="basic-cart-cart basic-cart-grid">',
        '#suffix' => '</div>'
    );
    // Cart elements.
    foreach ($cart as $cid => $value) {
        if (is_numeric($cid)) {
            $node                       = node_load($cart[$value]);
            $form['cartcontents'][$cid] = array(
                '#type' => 'numberfield',
                '#size' => 1,
                '#default_value' => $_SESSION['basic_cart_plus']['cart'][$cid . 'quantity'],
                '#theme' => 'basic_cart_plus_render_cart_element'
            );
        }
    }
    // Total price.
    $form['total_price']         = array(
        '#markup' => t('Total price'),
        '#prefix' => '<div class="basic-cart-cart basic-cart-grid">',
        '#suffix' => '</div>',
        '#theme' => 'basic_cart_plus_cart_total_price'
    );
    // Buttons.
    $form['buttons']             = array(
        // Make the returned array come back in tree form.
        '#tree' => TRUE,
        '#prefix' => '<div class="row"><div class="basic-cart-call-to-action cell">',
        '#suffix' => '</div></div>'
    );
    // Update button.
    $form['buttons']['update']   = array(
        '#type' => 'submit',
        '#value' => t('Update')
    );
    // Checkout button.
    $form['buttons']['checkout'] = array(
        '#type' => 'submit',
        '#value' => t('Checkout')
    );
    return $form;
}
/**
 * Shopping cart form.
 */
function basic_cart_plus_cart_form_submit($form_id, $form_state)
{
    foreach ($form_state['values']['cartcontents'] as $nid => $value) {
        $quantity = (int) $value;
        if ($quantity > 0) {
            $_SESSION['basic_cart_plus']['cart'][$nid . 'quantity'] = $quantity;
        }
        // If the quantity is zero, we just remove the node from the cart.
        elseif ($quantity == 0) {
            unset($_SESSION['basic_cart_plus']['cart'][$nid]);
            unset($_SESSION['basic_cart_plus']['cart'][$nid . 'quantity']);
        }
    }
    if ($form_state['values']['op'] == t('Checkout')) {
        backdrop_goto('checkout');
    } else {
        backdrop_set_message(t('Shopping cart updated.'));
    }
}
/**
 * Callback function for cart/add/.
 *
 * @param int $nid
 *   We are using the node id to store the node in the shopping cart
 */
function basic_cart_plus_add_to_cart($nid = NULL)
{
    $nid         = (int) $nid;
    $node        = node_load($nid);
    $member_info = basic_cart_plus_node_membership_info($node);
    if (($member_info["is_member_node"] > 0 && $member_info["is_member_user"] == 0) || ($member_info["is_bronze_node"] > 0 && $member_info["is_bronze_user"] == 0) || ($member_info["is_silver_node"] > 0 && $member_info["is_silver_user"] == 0) || ($member_info["is_gold_node"] > 0 && $member_info["is_gold_user"] == 0)) {
        basic_cart_plus_empty_cart();
    }

    if ($nid > 0) {
        // If a node is added more times, just update the quantity.
        $cart = basic_cart_plus_get_cart();

        foreach ($cart as $cid => $value) {
            if (is_numeric($cid)) {
                $node        = node_load($cart[$value]);
                $member_info = basic_cart_plus_node_membership_info($node);
                if ($member_info["is_member_node"] > 0 || $member_info["is_bronze_node"] > 0 || $member_info["is_silver_node"] > 0 || $member_info["is_gold_node"] > 0) {
                    basic_cart_plus_empty_cart();
                }

            }
        }

        if (!empty($cart) && in_array($nid, array_keys($cart))) {
            // Clicked 2 times on add to cart button. Increment quantity.
            $x = $_SESSION['basic_cart_plus']['cart'][$nid . 'quantity'];
            $x++;
            $_SESSION['basic_cart_plus']['cart'][$nid . 'quantity'] = $x;
        } else {
            // Storing in session.
            $_SESSION['basic_cart_plus']['cart'][$nid]              = $nid;
            $_SESSION['basic_cart_plus']['cart'][$nid . 'quantity'] = 1;
        }
    }
    backdrop_set_message(t('Shopping cart updated.'));
    $redirect = config_get('basic_cart_plus.settings', 'basic_cart_plus_redirect_user_after_add_to_cart');
    if (empty($redirect)) {
        backdrop_goto('cart');
    } elseif ($redirect == '<none>') {
        $referer = $_SERVER["HTTP_REFERER"];
        backdrop_goto($referer);
    } else {
        backdrop_goto($redirect);
    }
}
/**
 * Callback function for cart/remove/.
 *
 * @param int $nid
 *   We are using the node id to remove the node in the shopping cart
 */
function basic_cart_plus_remove_from_cart($nid = NULL)
{
    $nid = (int) $nid;
    if ($nid > 0) {
        unset($_SESSION['basic_cart_plus']['cart'][$nid]);
        unset($_SESSION['basic_cart_plus']['cart'][$nid . 'quantity']);
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
function basic_cart_plus_get_cart($nid = NULL)
{
    if (isset($nid)) {
        return $_SESSION['basic_cart_plus']['cart'][$nid];
    }
    if (isset($_SESSION['basic_cart_plus']['cart'])) {
        return $_SESSION['basic_cart_plus']['cart'];
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
function basic_cart_plus_get_total_price()
{
    // Building the return array.
    $return = array(
        'price' => 0,
        'vat' => 0,
        'total' => 0
    );
    $cart   = basic_cart_plus_get_cart();
    if (empty($cart)) {
        return (object) $return;
    }
    $total_price = 0;
    foreach ($cart as $cid => $value) {
        if (is_numeric($cid)) {
            $node = node_load($cart[$value]);

            $member_info = basic_cart_plus_node_membership_info($node);
            if ($member_info["is_member_node"] > 0 && $member_info["is_member_user"] == 0) {
                $total_price += config_get('basic_cart_plus.settings', "basic_cart_plus_paywall_price");
            }

            if ($member_info["is_bronze_node"] > 0 && $member_info["is_bronze_user"] == 0) {
                $total_price += config_get('basic_cart_plus.settings', "basic_cart_plus_paywall_bronze_price");
            }

            if ($member_info["is_silver_node"] > 0 && $member_info["is_silver_user"] == 0) {
                $total_price += config_get('basic_cart_plus.settings', "basic_cart_plus_paywall_silver_price");
            }

            if ($member_info["is_gold_node"] > 0 && $member_info["is_gold_user"] == 0) {
                $total_price += config_get('basic_cart_plus.settings', "basic_cart_plus_paywall_gold_price");
            }

            if (isset($_SESSION['basic_cart_plus']['cart'][$cid . 'quantity']) && isset($node->price["und"][0]["value"])) {
                $total_price += $_SESSION['basic_cart_plus']['cart'][$cid . 'quantity'] * $node->price["und"][0]["value"];
            }
        }
    }
    $return['price'] = $total_price;
    // Checking whether to apply the VAT or not.
    $vat_is_enabled  = (int) config_get('basic_cart_plus.settings', 'basic_cart_plus_vat_state');
    if (!empty($vat_is_enabled) && $vat_is_enabled) {
        $vat_value = (float) config_get('basic_cart_plus.settings', 'basic_cart_plus_vat_value');
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
function basic_cart_plus_empty_cart()
{
    unset($_SESSION['basic_cart_plus']['cart']);
}
/**
 * Formats the input $price in the desired format.
 *
 * @param float $price
 *   The price in the raw format.
 * @return $price
 *   The price in the custom format.
 */
function basic_cart_plus_price_format($price)
{
    $format   = config_get('basic_cart_plus.settings', 'basic_cart_plus_price_format');
    $currency = check_plain(config_get('basic_cart_plus.settings', 'basic_cart_plus_currency'));
    $price    = (float) $price;

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
function _basic_cart_plus_price_format()
{
    $currency = config_get('basic_cart_plus.settings', 'basic_cart_plus_currency');
    return array(
        0 => t('1 234,00 @currency', array(
            '@currency' => $currency
        )),
        1 => t('1 234.00 @currency', array(
            '@currency' => $currency
        )),
        2 => t('1,234.00 @currency', array(
            '@currency' => $currency
        )),
        3 => t('1.234,00 @currency', array(
            '@currency' => $currency
        )),
        4 => t('@currency 1 234,00', array(
            '@currency' => $currency
        )),
        5 => t('@currency 1 234.00', array(
            '@currency' => $currency
        )),
        6 => t('@currency 1,234.00', array(
            '@currency' => $currency
        )),
        7 => t('@currency 1.234,00', array(
            '@currency' => $currency
        ))
    );
}
/**
 * Checkout form implementation.
 */
function basic_cart_plus_checkout()
{
    $shopping_cart  = basic_cart_plus_get_cart();
    // Price.
    $price          = basic_cart_plus_get_total_price();
    $total          = basic_cart_plus_price_format($price->total);
    $options        = array(
        'cart' => $shopping_cart,
        'price' => $total
    );
    // Checking the VAT.
    $vat_is_enabled = (int) config_get('basic_cart_plus.settings', 'basic_cart_plus_vat_state');
    if (!empty($vat_is_enabled) && $vat_is_enabled) {
        $options['vat'] = basic_cart_plus_price_format($price->vat);
    }
    // The flat cart (just the listing part).
    $cart = theme('basic_cart_plus_cart_flat', $options);
    // If the cart is empty, we don't want to show the checkout form.
    if (empty($shopping_cart)) {
        return $cart;
    }
    $form = backdrop_get_form('basic_cart_plus_create_form');
    $form = backdrop_render($form);
    return $cart . $form;
}


/**
 * Implements hook_mail().
 */
function basic_cart_plus_mail($key, &$message, $params)
{
    switch ($key) {
        case 'admin_mail':
            $message['subject'] = config_get('basic_cart_plus.settings', 'basic_cart_plus_admin_subject');
            $message['body'][]  = filter_xss($params['admin_message']);
            break;
        case 'user_mail':
            $message['subject'] = config_get('basic_cart_plus.settings', 'basic_cart_plus_user_subject');
            $message['body'][]  = filter_xss($params['user_message']);
            break;
    }
}
/**
 * Callback for thank you page.
 */
function basic_cart_plus_checkout_thank_you()
{
    $title = config_get('basic_cart_plus.settings', 'basic_cart_plus_thank_you_title');
    backdrop_set_title($title);
    $message = config_get('basic_cart_plus.settings', 'basic_cart_plus_thank_you_message');
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
function basic_cart_plus_create_form($form, $form_state)
{
    // Return payment section of form if page_num == 2. basic_cart_form_payment
    if (!empty($form_state['page_num']) && $form_state['page_num'] == 2) {
        $which_payment = config_get('basic_cart_plus.settings', 'basic_cart_plus_which_payment');
        if ($which_payment == 0) {
            return basic_cart_plus_form_payment($form, $form_state);
        }
        if ($which_payment == 1) {
            return basic_cart_plus_stripe_form_payment($form, $form_state);
        }
        if ($which_payment == 2) {
            return basic_cart_plus_braintree_form_payment($form, $form_state);
        }

    }
    // Set page_num to 1 and build page 1.
    $form_state['page_num'] = 1;
    $form                   = array();
    // Define form as hierarchy so we can access both pages as trees.
    $form['#tree']          = TRUE;
    $form['billing']        = array(
        '#type' => 'fieldset',
        '#title' => t('Billing & Account Details - Enter your account, or this site will create one for you.'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE
    );
    // Check for $form_state values returned on form rebuild and when submitting the back button.
    global $user;
    if ($user->uid == 0) {
        $email                            = isset($form_state['values']['billing']['email']) ? $form_state['values']['billing']['email'] : '';
        $form['billing']['email']         = array(
            '#type' => 'emailfield',
            '#title' => t('Email'),
            '#required' => TRUE,
            '#default_value' => $email,
            // Perform ajax validation.
            '#ajax' => array(
                'callback' => '_basic_cart_plus_form_email_ajax_validate',
                'wrapper' => 'email-error',
                'effect' => 'slide'
            ),
            '#suffix' => '<div id="email-error" style="color:#ff0000;"> </div>'
        );
        $form['billing']['password']      = array(
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
            '#type' => 'emailfield',
            '#title' => t('Email'),
            '#required' => TRUE,
            '#default_value' => $email,
            // Perform ajax validation.
            '#ajax' => array(
                'callback' => '_basic_cart_plus_form_email_ajax_validate',
                'wrapper' => 'email-error',
                'effect' => 'slide'
            ),
            '#suffix' => '<div id="email-error" style="color:#ff0000;"> </div>'
        );
    }

    global $user;
    $current_user = user_load($user->uid);

    // first name
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_firstname');
    if ($testvariable > 0) {

        if (isset($current_user->field_basic_cart_plus_firstname[$current_user->langcode])) {
            $savedfirstname = $current_user->field_basic_cart_plus_firstname[$current_user->langcode]['0']['value'];
        } else {
            $savedfirstname = "";
        }
        $firstname                    = isset($form_state['values']['billing']['firstname']) ? $form_state['values']['billing']['firstname'] : $savedfirstname;
        $form['billing']['firstname'] = array(
            '#type' => 'textfield',
            '#title' => t('First Name'),
            '#required' => TRUE,
            '#default_value' => $firstname
        );
    }

    // last name
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_lastname');
    if ($testvariable > 0) {

        if (isset($current_user->field_basic_cart_plus_lastname[$current_user->langcode])) {
            $savedlastname = $current_user->field_basic_cart_plus_lastname[$current_user->langcode]['0']['value'];
        } else {
            $savedlastname = "";
        }
        $lastname                    = isset($form_state['values']['billing']['lastname']) ? $form_state['values']['billing']['lastname'] : $savedlastname;
        $form['billing']['lastname'] = array(
            '#type' => 'textfield',
            '#title' => t('Last Name'),
            '#required' => TRUE,
            '#default_value' => $lastname
        );
    }

    // phone
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_phone');
    if ($testvariable > 0) {
        if (isset($current_user->field_basic_cart_plus_phone[$current_user->langcode])) {
            $savedphone = $current_user->field_basic_cart_plus_phone[$current_user->langcode]['0']['value'];
        } else {
            $savedphone = "";
        }
        $phone                    = isset($form_state['values']['billing']['phone']) ? $form_state['values']['billing']['phone'] : $savedphone;
        $form['billing']['phone'] = array(
            '#type' => 'telfield',
            '#title' => t('Phone'),
            '#required' => TRUE,
            '#default_value' => $phone
        );
    }

    // address 1
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_address');
    if ($testvariable > 0) {
        if (isset($current_user->field_basic_cart_plus_address[$current_user->langcode])) {
            $savedaddress = $current_user->field_basic_cart_plus_address[$current_user->langcode]['0']['value'];
        } else {
            $savedaddress = "";
        }
        $address                    = isset($form_state['values']['billing']['address']) ? $form_state['values']['billing']['address'] : $savedaddress;
        $form['billing']['address'] = array(
            '#type' => 'textfield',
            '#title' => t('Address Line 1'),
            '#required' => TRUE,
            '#default_value' => $address
        );
    }

    // address 2
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_address2');
    if ($testvariable > 0) {
        if (isset($current_user->field_basic_cart_plus_address2[$current_user->langcode])) {
            $savedaddress2 = $current_user->field_basic_cart_plus_address2[$current_user->langcode]['0']['value'];
        } else {
            $savedaddress2 = "";
        }
        $address2                    = isset($form_state['values']['billing']['address2']) ? $form_state['values']['billing']['address2'] : $savedaddress2;
        $form['billing']['address2'] = array(
            '#type' => 'textfield',
            '#title' => t('Address Line 2'),
            '#default_value' => $address2
        );
    }

    // city
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_city');
    if ($testvariable > 0) {
        if (isset($current_user->field_basic_cart_plus_city[$current_user->langcode])) {
            $savedcity = $current_user->field_basic_cart_plus_city[$current_user->langcode]['0']['value'];
        } else {
            $savedcity = "";
        }
        $city                    = isset($form_state['values']['billing']['city']) ? $form_state['values']['billing']['city'] : $savedcity;
        $form['billing']['city'] = array(
            '#type' => 'textfield',
            '#title' => t('City'),
            '#required' => TRUE,
            '#default_value' => $city
        );
    }

    // county
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_county');
    if ($testvariable > 0) {
        if (isset($current_user->field_basic_cart_plus_county[$current_user->langcode])) {
            $savedcounty = $current_user->field_basic_cart_plus_county[$current_user->langcode]['0']['value'];
        } else {
            $savedcounty = "";
        }
        $county                    = isset($form_state['values']['billing']['county']) ? $form_state['values']['billing']['county'] : $savedcounty;
        $form['billing']['county'] = array(
            '#type' => 'textfield',
            '#title' => t('County'),
            '#required' => TRUE,
            '#default_value' => $county
        );
    }

    // state
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_state');
    if ($testvariable > 0) {
        if (isset($current_user->field_basic_cart_plus_state[$current_user->langcode])) {
            $savedstate = $current_user->field_basic_cart_plus_state[$current_user->langcode]['0']['value'];
        } else {
            $savedstate = "";
        }
        $state                    = isset($form_state['values']['billing']['state']) ? $form_state['values']['billing']['state'] : $savedstate;
        $form['billing']['state'] = array(
            '#type' => 'textfield',
            '#title' => t('State'),
            '#required' => TRUE,
            '#default_value' => $state
        );
    }

    // zip
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_zip');
    if ($testvariable > 0) {
        if (isset($current_user->field_basic_cart_plus_zip[$current_user->langcode])) {
            $savedzip = $current_user->field_basic_cart_plus_zip[$current_user->langcode]['0']['value'];
        } else {
            $savedzip = "";
        }
        $zip                    = isset($form_state['values']['billing']['zip']) ? $form_state['values']['billing']['zip'] : $savedzip;
        $form['billing']['zip'] = array(
            '#type' => 'telfield',
            '#title' => t('Zip'),
            '#required' => TRUE,
            '#size' => 10,
            '#maxlength' => 10,
            '#default_value' => $zip
        );
    }

    // country
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_country');
    if ($testvariable > 0) {
        if (isset($current_user->field_basic_cart_plus_country[$current_user->langcode])) {
            $savedcountry = $current_user->field_basic_cart_plus_country[$current_user->langcode]['0']['value'];
        } else {
            $savedcountry = "";
        }
        $country                    = isset($form_state['values']['billing']['country']) ? $form_state['values']['billing']['country'] : $savedcountry;
        $form['billing']['country'] = array(
            '#type' => 'textfield',
            '#title' => t('Country'),
            '#required' => TRUE,
            '#size' => 20,
            '#maxlength' => 20,
            '#default_value' => $country
        );
    }

    // company
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_company');
    if ($testvariable > 0) {
        if (isset($current_user->field_basic_cart_plus_company[$current_user->langcode])) {
            $savedcompany = $current_user->field_basic_cart_plus_company[$current_user->langcode]['0']['value'];
        } else {
            $savedcompany = "";
        }
        $company                    = isset($form_state['values']['billing']['company']) ? $form_state['values']['billing']['company'] : $savedcompany;
        $form['billing']['company'] = array(
            '#type' => 'textfield',
            '#title' => t('Company'),
            '#required' => TRUE,
            '#size' => 20,
            '#maxlength' => 20,
            '#default_value' => $company
        );
    }

    // website
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_website');
    if ($testvariable > 0) {
        if (isset($current_user->field_basic_cart_plus_website[$current_user->langcode])) {
            $savedwebsite = $current_user->field_basic_cart_plus_website[$current_user->langcode]['0']['value'];
        } else {
            $savedwebsite = "";
        }
        $website                    = isset($form_state['values']['billing']['website']) ? $form_state['values']['billing']['website'] : $savedwebsite;
        $form['billing']['website'] = array(
            '#type' => 'urlfield',
            '#title' => t('Website'),
            '#required' => TRUE,
            '#size' => 25,
            '#maxlength' => 25,
            '#default_value' => $website
        );
    }

    // comment
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_comment');
    if ($testvariable > 0) {
        $comment                    = isset($form_state['values']['billing']['comment']) ? $form_state['values']['billing']['comment'] : '';
        $form['billing']['comment'] = array(
            '#type' => 'textarea',
            '#title' => t('Comment'),
            '#required' => TRUE,
            '#size' => 100,
            '#maxlength' => 255,
            '#default_value' => $comment
        );
    }
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Next',
        '#submit' => array(
            'basic_cart_plus_form_tracker'
        ),
        '#validate' => array(
            'basic_cart_plus_form_billing_validate'
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
function basic_cart_plus_form_tracker($form, &$form_state)
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
function basic_cart_plus_form_back($form, &$form_state)
{
    $form_state['values']   = $form_state['page_values'][1];
    $form_state['page_num'] = 1;
    $form_state['rebuild']  = TRUE;
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
function _basic_cart_plus_form_email_ajax_validate($form, &$form_state)
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
function basic_cart_plus_check_customer($email, $password)
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
        $array          = explode("@", $email);
        $customername   = $array[0];
        $user           = entity_create('user', array());
        $user->name     = $customername;
        $user->pass     = $password;
        $user->mail     = $email;
        $user->status   = 1;
        $user->access   = 0;
        $user->login    = 0;
        $user->timezone = config_get('system.core', 'date_default_timezone');
        $user->data     = FALSE;
        $user->save();
        $user_obj          = user_load_by_name($customername);
        $form_state        = array();
        $form_state['uid'] = $user_obj->uid;
        user_login_submit(array(), $form_state);
        // user_login_finalize();
        return $user_obj->uid;
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
function basic_cart_plus_form_billing_validate($form, $form_state)
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

function basic_cart_plus_sanitize_data($billing_values)
{
    $email = filter_var($billing_values['billing']['email'], FILTER_SANITIZE_EMAIL);

    global $user;
    if ($user->uid == 0) {
        $customer_id = basic_cart_plus_check_customer($billing_values['billing']['email'], $billing_values['billing']['password']);
        // $original_user = $user;
        // $old_state = backdrop_save_session();
        // backdrop_save_session(FALSE);
        $user        = user_load($customer_id);
    } else {
        $user = user_load($user->uid);
    }
    if (!empty($billing_values['billing']['firstname'])) {
        $firstname = preg_replace('/[^a-zA-Z-\.^\s\p{L}]/u', '', $billing_values['billing']['firstname']);
        if (empty($firstname)) {
            $firstname = "default";
        }
    } else {
        if (isset($user->field_basic_cart_plus_firstname[$user->langcode])) {
            $firstname = $user->field_basic_cart_plus_firstname[$user->langcode][0]['value'];
        } else {
            $firstname = "default";
        }
    }
    $user->field_basic_cart_plus_firstname[$user->langcode][0]['value'] = $firstname;

    if (!empty($billing_values['billing']['lastname'])) {
        $lastname = preg_replace('/[^a-zA-Z-\.^\s\p{L}]/u', '', $billing_values['billing']['lastname']);
        if (empty($lastname)) {
            $lastname = "default";
        }
    } else {
        if (isset($user->field_basic_cart_plus_lastname[$user->langcode])) {
            $lastname = $user->field_basic_cart_plus_lastname[$user->langcode][0]['value'];
        } else {
            $lastname = "default";
        }
    }
    $user->field_basic_cart_plus_lastname[$user->langcode][0]['value'] = $lastname;

    if (!empty($billing_values['billing']['phone'])) {
        $phone = preg_replace('/[^0-9-]/i', '', $billing_values['billing']['phone']);
        if (empty($phone)) {
            $phone = "123";
        }
    } else {
        if (isset($user->field_basic_cart_plus_phone[$user->langcode])) {
            $phone = $user->field_basic_cart_plus_phone[$user->langcode][0]['value'];
        } else {
            $phone = "123";
        }
    }
    $user->field_basic_cart_plus_phone[$user->langcode][0]['value'] = $phone;

    if (!empty($billing_values['billing']['address'])) {
        $address = preg_replace('/[^0-9a-zA-Z-\.^\s\p{L}]/u', '', $billing_values['billing']['address']);
        if (empty($address)) {
            $address = "default";
        }
    } else {
        if (isset($user->field_basic_cart_plus_address[$user->langcode])) {
            $address = $user->field_basic_cart_plus_address[$user->langcode][0]['value'];
        } else {
            $address = "default";
        }
    }
    $user->field_basic_cart_plus_address[$user->langcode][0]['value'] = $address;

    if (!empty($billing_values['billing']['address2'])) {
        $address2 = preg_replace('/[^0-9a-zA-Z-\.^\s\p{L}]/u', '', $billing_values['billing']['address2']);
        if (empty($address2)) {
            $address2 = "default";
        }
    } else {
        if (isset($user->field_basic_cart_plus_address2[$user->langcode])) {
            $address2 = $user->field_basic_cart_plus_address2[$user->langcode][0]['value'];
        } else {
            $address2 = "default";
        }
    }
    $user->field_basic_cart_plus_address2[$user->langcode][0]['value'] = $address2;

    if (!empty($billing_values['billing']['city'])) {
        $city = preg_replace('/[^a-z-\.]/i', '', $billing_values['billing']['city']);
        if (empty($city)) {
            $city = "default";
        }
    } else {
        if (isset($user->field_basic_cart_plus_city[$user->langcode])) {
            $city = $user->field_basic_cart_plus_city[$user->langcode][0]['value'];
        } else {
            $city = "default";
        }
    }
    $user->field_basic_cart_plus_city[$user->langcode][0]['value'] = $city;

    if (!empty($billing_values['billing']['state'])) {
        $state = preg_replace('/[^a-z]/i', '', $billing_values['billing']['state']);
        if (empty($state)) {
            $state = "default";
        }
    } else {
        if (isset($user->field_basic_cart_plus_state[$user->langcode])) {
            $state = $user->field_basic_cart_plus_state[$user->langcode][0]['value'];
        } else {
            $state = "default";
        }
    }
    $user->field_basic_cart_plus_state[$user->langcode][0]['value'] = $state;

    if (!empty($billing_values['billing']['zip'])) {
        $zip = preg_replace('/[^0-9]/i', '', $billing_values['billing']['zip']);
        if (empty($zip)) {
            $zip = "123";
        }
    } else {
        if (isset($user->field_basic_cart_plus_zip[$user->langcode])) {
            $zip = $user->field_basic_cart_plus_zip[$user->langcode][0]['value'];
        } else {
            $zip = "123";
        }
    }
    $user->field_basic_cart_plus_zip[$user->langcode][0]['value'] = $zip;

    if (!empty($billing_values['billing']['county'])) {
        $county = preg_replace('/[^a-zA-Z-\.^\s\p{L}]/u', '', $billing_values['billing']['county']);
        if (empty($county)) {
            $county = "default";
        }
    } else {
        if (isset($user->field_basic_cart_plus_county[$user->langcode])) {
            $county = $user->field_basic_cart_plus_county[$user->langcode][0]['value'];
        } else {
            $county = "default";
        }
    }
    $user->field_basic_cart_plus_county[$user->langcode][0]['value'] = $county;

    if (!empty($billing_values['billing']['country'])) {
        $country = preg_replace('/[^a-zA-Z-\.^\s\p{L}]/u', '', $billing_values['billing']['country']);
        if (empty($country)) {
            $country = "USA";
        }
    } else {
        if (isset($user->field_basic_cart_plus_country[$user->langcode])) {
            $country = $user->field_basic_cart_plus_country[$user->langcode][0]['value'];
        } else {
            $country = "USA";
        }
    }
    $user->field_basic_cart_plus_country[$user->langcode][0]['value'] = $country;

    if (!empty($billing_values['billing']['company'])) {
        $company = preg_replace('/[^a-zA-Z-\.^\s\p{L}]/u', '', $billing_values['billing']['company']);
        if (empty($company)) {
            $company = "default";
        }
    } else {
        if (isset($user->field_basic_cart_plus_company[$user->langcode])) {
            $company = $user->field_basic_cart_plus_company[$user->langcode][0]['value'];
        } else {
            $company = "default";
        }
    }
    $user->field_basic_cart_plus_company[$user->langcode][0]['value'] = $company;

    if (!empty($billing_values['billing']['website'])) {
        $website = filter_var($billing_values['billing']['website'], FILTER_SANITIZE_URL);
        if (empty($website)) {
            $website = "http://www.example.com";
        }
    } else {
        if (isset($user->field_basic_cart_plus_website[$user->langcode])) {
            $website = $user->field_basic_cart_plus_website[$user->langcode][0]['value'];
        } else {
            $website = "http://www.example.com";
        }
    }
    $user->field_basic_cart_plus_website[$user->langcode][0]['value'] = $website;

    if (!empty($billing_values['billing']['comment'])) {
        $comment = filter_var($billing_values['billing']['comment'], FILTER_SANITIZE_STRING);
        // $comment = filter_var($comment, FILTER_SANITIZE_ENCODED);
        $comment = filter_var($comment, FILTER_SANITIZE_SPECIAL_CHARS);
    } else {
        $comment = "default";
    }

    $user->save();
    return $comment;
}


function basic_cart_plus_form_payment($form, $form_state)
{
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
        '#submit' => array(
            'basic_cart_plus_form_callback'
        ),
        '#attributes' => array(
            'id' => array(
                'stripesubmit'
            )
        )
    );
    $form['back']   = array(
        '#type' => 'submit',
        '#value' => t('<< Back'),
        '#submit' => array(
            'basic_cart_plus_form_back'
        ),
        '#limit_validation_errors' => array()
    );
    return $form;
}






function basic_cart_plus_form_callback($form, $form_state)
{
    // Assign billing data for easier referencing.
    $billing_values = $form_state['page_values']['1'];
    $comment        = basic_cart_plus_sanitize_data($billing_values);
    $order_details  = '';
    $cart           = basic_cart_plus_get_cart();
    $price          = basic_cart_plus_get_total_price();
    $total_price    = basic_cart_plus_price_format($price->total);

    $vat_is_enabled = (int) config_get('basic_cart_plus.settings', 'basic_cart_plus_vat_state');
    if (!empty($vat_is_enabled) && $vat_is_enabled) {
        $vat = basic_cart_plus_price_format($price->vat);
    }

    // Building the order details.
    $i = 0;
    // Cart elements.
    foreach ($cart as $cid => $value) {
        if (is_numeric($cid)) {
            $node        = node_load($cart[$value]);
            $member_info = basic_cart_plus_node_membership_info($node);
            if (($member_info["is_member_node"] > 0 && $member_info["is_member_user"] == 0) || ($member_info["is_bronze_node"] > 0 && $member_info["is_bronze_user"] == 0) || ($member_info["is_silver_node"] > 0 && $member_info["is_silver_user"] == 0) || ($member_info["is_gold_node"] > 0 && $member_info["is_gold_user"] == 0)) {
                $order_details .= ++$i . ". " . $node->title . "\t 1 x " . $total_price . "\n";
                $sitemembership = "yes";
                global $user;
                $uid = $user->uid; // User ID of user that you want to add role to.
            }

            if ($member_info["is_member_node"] > 0 && $member_info["is_member_user"] == 0) {
                user_multiple_role_edit(array(
                    $uid
                ), 'add_role', "paywall_member");
            }

            if ($member_info["is_bronze_node"] > 0 && $member_info["is_bronze_user"] == 0) {
                user_multiple_role_edit(array(
                    $uid
                ), 'add_role', "paywall_bronze");
            }

            if ($member_info["is_silver_node"] > 0 && $member_info["is_silver_user"] == 0) {
                user_multiple_role_edit(array(
                    $uid
                ), 'add_role', "paywall_silver");
            }

            if ($member_info["is_gold_node"] > 0 && $member_info["is_gold_user"] == 0) {
                user_multiple_role_edit(array(
                    $uid
                ), 'add_role', "paywall_gold");
            }

            if (!isset($sitemembership)) {
                $unit_price = basic_cart_plus_price_format($node->price["und"][0]["value"]);
                $order_details .= ++$i . '. ' . $node->title . "\t" . $_SESSION['basic_cart_plus']['cart'][$cid . 'quantity'] . ' x ' . $unit_price . "\n";
            }


        }
    }
    $order_details .= "\n";
    $order_details .= "Total price: " . $total_price;
    $vat_is_enabled = (int) config_get('basic_cart_plus.settings', 'basic_cart_plus_vat_state');
    if ($vat_is_enabled) {
        $order_details .= "\n";
        $order_details .= "Total VAT: " . $vat;
    }
    $total_price = $total_price * 1;

    // Registering the new order to the database.
    if (module_exists('basic_cart_plus_order')) {
        basic_cart_plus_save_order($comment);
    }

    basic_cart_plus_mail_order($comment, $total_price);
    basic_cart_plus_empty_cart();
    backdrop_goto('checkout/thank-you');
}

function basic_cart_plus_save_order($comment)
{
    $order = basic_cart_plus_order_register_order($comment);
    // legacy custom implementation
    // if you want to use a hook for other modules, uncomment, make a global user and set the order details variable etc...
    // $name = $user->$firstname . " " . $lastname;
    // $order_details_ = array(
    //         'phone' => $phone,
    //         'address' => $address,
    //         'address2' => $address2,
    //         'city' => $city,
    //         'county' => $county,
    //         'state' => $state,
    //         'zipcode' => $zip,
    //         'country' => $country,
    //         'company' => $company,
    //         'website' => $website,
    //         'comment' => $comment
    //     );
    //     $order          = basic_cart_plus_order_register_order($name, $email, $order_details_);
    //     // Order successfully placed. Custom hook for other modules.
    //     foreach (module_implements('basic_cart_plus_order') as $module) {
    //         $function = $module . '_basic_cart_plus_order';
    //         // Will call all modules implementing hook_basic_cart_stripe_order() with the order node as argument.
    //         $function($order);
    //     }
}

function basic_cart_plus_mail_order($comment, $total_price)
{
    // Pleaceholder replacement.
    $search = array(
        '%CUSTOMER_NAME',
        '%CUSTOMER_EMAIL',
        '%CUSTOMER_PHONE',
        '%CUSTOMER_ADDRESS',
        '%CUSTOMER_ADDRESS2',
        '%CUSTOMER_CITY',
        '%CUSTOMER_COUNTY',
        '%CUSTOMER_STATE',
        '%CUSTOMER_ZIPCODE',
        '%CUSTOMER_COUNTRY',
        '%CUSTOMER_COMPANY',
        '%CUSTOMER_WEBSITE',
        '%CUSTOMER_COMMENT'
    );

    global $user;
    $user                    = user_load($user->uid);
    $name                    = $user->field_basic_cart_plus_firstname[$user->langcode][0]['value'] . " " . $user->field_basic_cart_plus_lastname[$user->langcode][0]['value'];
    $replace                 = array(
        $name,
        $user->mail,
        $user->field_basic_cart_plus_phone[$user->langcode][0]['value'],
        $user->field_basic_cart_plus_address[$user->langcode][0]['value'],
        $user->field_basic_cart_plus_address2[$user->langcode][0]['value'],
        $user->field_basic_cart_plus_city[$user->langcode][0]['value'],
        $user->field_basic_cart_plus_county[$user->langcode][0]['value'],
        $user->field_basic_cart_plus_state[$user->langcode][0]['value'],
        $user->field_basic_cart_plus_zip[$user->langcode][0]['value'],
        $user->field_basic_cart_plus_country[$user->langcode][0]['value'],
        $user->field_basic_cart_plus_company[$user->langcode][0]['value'],
        $user->field_basic_cart_plus_website[$user->langcode][0]['value'],
        $comment
    );
    // Admin message.
    $message_html            = config_get('basic_cart_plus.settings', 'basic_cart_plus_admin_message');
    $message_html            = str_replace($search, $replace, $message_html);
    // Admin mail.
    $params['admin_message'] = $message_html;
    $site_mail               = config_get('system.core', 'site_mail');
    $admin_emails            = config_get('basic_cart_plus.settings', 'basic_cart_plus_admin_emails');
    if (empty($admin_emails)) {
        // Sending mail to admin.
        $message    = backdrop_mail('basic_cart_plus', 'admin_mail', $site_mail, language_default(), $params);
        $mails_sent = 0;
        if ($message['result']) {
            $mails_sent++;
        }
    } else {
        $admin_emails = explode("\n", $admin_emails);
        if (is_array($admin_emails) && !empty($admin_emails)) {
            $ok = FALSE;
            foreach ($admin_emails as $admin_email) {
                // Sending mail to each admin.
                $message = backdrop_mail('basic_cart_plus', 'admin_mail', $admin_email, language_default(), $params);
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
    $send_user_mail = config_get('basic_cart_plus.settings', 'basic_cart_plus_send_user_message');
    if ($send_user_mail) {
        $message_html           = config_get('basic_cart_plus.settings', 'basic_cart_plus_user_message');
        $message_html           = str_replace($search, $replace, $message_html);
        $params['user_message'] = $message_html;
        // Sending mail.
        $message                = backdrop_mail('basic_cart_plus', 'user_mail', $user->mail, language_default(), $params);
        if ($message['result']) {
            $mails_sent++;
        }
    }
    if ($mails_sent >= 1) {

    } else {
        backdrop_set_message(t('There was a problem in submitting your order. Please try again later.'), 'error');
    }
    $testvariable = config_get('basic_cart_plus.settings', 'basic_cart_plus_sendemail');
    if ($testvariable > 0) {
        $module                   = 'basic_cart_plus';
        $key                      = 'key';
        $to                       = $user->mail;
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
        $message['body'][]        = $to . ", login to " . $_SERVER['HTTP_HOST'] . $_SERVER['REQUEST_URI'] . " to see your order for " . $total_price . " on " . date('l jS \of F Y h:i:s A');
        // Retrieve the responsible implementation for this message.
        $system                   = backdrop_mail_system($module, $key);
        // Format the message body.
        $message                  = $system->format($message);
        // Send e-mail.
        $message['result']        = $system->mail($message);
        if ($message['result'] == TRUE) {
            backdrop_set_message(t('Your receipt has been sent. Thank you!'));
        } else {
            backdrop_set_message(t('There was a problem sending your receipt and it was not sent.'), 'error');
        }
    }

}
