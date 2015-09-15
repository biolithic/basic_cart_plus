<?php
/**
 * @file
 * Basic Cart Plus shopping cart implementation functions.
 */
/**
 * Provide page two of form to collect payment information and process with basic_cart_plus.
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
function basic_cart_plus_stripe_form_payment($form, $form_state)
{
    backdrop_add_js('https://js.stripe.com/v1/', 'external');
    backdrop_add_js(backdrop_get_path('module', 'basic_cart_plus_stripe') . '/js/basic_cart_plus_stripe.js');
    $form['payment']                = array(
        '#type' => 'fieldset',
        '#title' => t('Payment Details'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE
    );
    $form['payment']['stripetoken'] = array(
        '#type' => 'hidden',
        '#title' => t('stripetoken')
    );
    $form['payment']['card']        = array(
        '#type' => 'telfield',
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
    $form['payment']['cvc']         = array(
        '#type' => 'telfield',
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
    $month_range                    = range('01', '12');
    $month_options                  = array_combine($month_range, $month_range);
    $form['payment']['month']       = array(
        '#type' => 'select',
        '#title' => t('Month'),
        '#options' => $month_options,
        '#attributes' => array(
            'class' => array(
                'card-expire-month'
            )
        )
    );
    $year_range                     = range(date('Y'), date('Y') + 5);
    $year_options                   = array_combine($year_range, $year_range);
    $form['payment']['year']        = array(
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
    $form['html']                   = array(
        '#type' => 'markup',
        '#markup' => '<a href="https://stripe.com" target="_blank"><img src="https://stripe.com/img/about/logos/badge/styled.png" border="0"/></a><br>For more information, see the <a href="https://stripe.com">Security Docs</a><br />'
    );
    $one                            = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_publishable_testkey');
    $two                            = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_publishable_livekey');
    $three                          = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_gonelive');
    if ($three > 0) {
        $publishable_api_key = $two;
    } else {
        $publishable_api_key = $one;
    }
    $form['basic_cart_stripe_publishable_key'] = array(
        '#type' => 'hidden',
        '#value' => $publishable_api_key,
        '#attributes' => array(
            'id' => array(
                'basic_cart_plus_stripe_publishable_key'
            )
        )
    );
    $form['submit']                            = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
        '#submit' => array(
            'basic_cart_plus_stripe_form_callback'
        ),
        '#attributes' => array(
            'id' => array(
                'stripesubmit'
            )
        )
    );
    $form['back']                              = array(
        '#type' => 'submit',
        '#value' => t('<< Back'),
        '#submit' => array(
            'basic_cart_plus_form_back'
        ),
        '#limit_validation_errors' => array()
    );
    $one                                       = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_publishable_testkey');
    $two                                       = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_publishable_livekey');
    $three                                     = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_gonelive');
    if ($three > 0) {
        $basic_cart_plus_stripe_publishable_key = $two;
    } else {
        $basic_cart_plus_stripe_publishable_key = $one;
    }
    $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_secret_testkey');
    $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_secret_livekey');
    $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_gonelive');
    if ($three > 0) {
        $basic_cart_plus_stripe_secret_key = $two;
    } else {
        $basic_cart_plus_stripe_secret_key = $one;
    }
    return $form;
}
/**
 * Process Stripe Charge and save user data.
 *
 * @param array $form
 * @param array $form_state
 */
function basic_cart_plus_stripe_form_callback($form, $form_state)
{

    $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_secret_testkey');
    $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_secret_livekey');
    $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_gonelive');
    if ($three > 0) {
        $basic_cart_plus_stripe_secret_key = $two;
    } else {
        $basic_cart_plus_stripe_secret_key = $one;
    }
    empty($basic_cart_plus_stripe_secret_key) ? NULL : \Stripe\Stripe::setApiKey($basic_cart_plus_stripe_secret_key);
    \Stripe\Stripe::setApiVersion(config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_setApiVersion'));

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
    foreach ($cart as $cid => $value) {
        if (is_numeric($cid)) {
            $node = node_load($cart[$value]);


            foreach (array_keys(node_type_get_names()) as $term) {
                $testvariable = "paywall_member_" . $term;
                $isenabled    = config_get('basic_cart_plus.settings', $testvariable);
                if ($node->type == $term && (arg(1) != $node->uid) && ($isenabled > 0) && !in_array("paywall_member", $user->roles)) {
                    $basic_cart_plus_paywall_price = config_get('basic_cart_plus.settings', "basic_cart_plus_paywall_price");
                    $order_details .= ++$i . ". " . $node->title . "\t 1 x " . $unit_price . "\n";
                    $sitemembership = "yes";
                }
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
    $total_price = $total_price * 100;


    $token = $form_state['values']['stripetoken'];
    try {
        // Create a Customer as referenced here https://stripe.com/docs/tutorials/charges.
        global $user;
        $current_user = user_load($user->uid);

        if (isset($current_user->field_basic_cart_plus_s_id[$current_user->langcode])) {
            $stripeid         = $current_user->field_basic_cart_plus_s_id[$current_user->langcode][0]['value'];
            $stripecustomer   = \Stripe\Customer::retrieve($stripeid);
            $stripecustomerid = $stripecustomer->id;
        } else {
            $stripecustomerid = "";
        }


        if (empty($stripecustomerid)) {
            $stripecustomer                                                                = \Stripe\Customer::create(array(
                "source" => $token,
                "description" => $billing_values['billing']['email'],
                "email" => $billing_values['billing']['email']
                // "currency" => config_get('basic_cart_plus.settings', 'basic_cart_plus_currency'), Stripe\Error\InvalidRequest: Received unknown parameter: currency
            ));
            $current_user->field_basic_cart_plus_s_id[$current_user->langcode][0]['value'] = $stripecustomer->id;
            $current_user->save();
        } else {
        }

        // Charge the customer.
        $charge = \Stripe\Charge::create(array(
            "amount" => $total_price,
            "currency" => config_get('basic_cart_plus.settings', 'basic_cart_plus_currency'),
            "customer" => $stripecustomer->id
        ));

        $uid = $user->uid; // User ID of user that you want to add role to.
        user_multiple_role_edit(array(
            $uid
        ), 'add_role', "paywall_member");

        $name = $current_user->field_basic_cart_plus_firstname[$current_user->langcode][0]['value'] . " " . $current_user->field_basic_cart_plus_lastname[$current_user->langcode][0]['value'];

        $stripecustomer->description                    = $current_user->mail;
        $stripecustomer->email                          = $current_user->mail;
        $stripecustomer->sources->data->name            = $name;
        $stripecustomer->sources->data->address_line1   = $current_user->field_basic_cart_address[$current_user->langcode][0]['value'];
        $stripecustomer->sources->data->address_line2   = $current_user->field_basic_cart_address2[$current_user->langcode][0]['value'];
        $stripecustomer->sources->data->address_city    = $current_user->field_basic_cart_plus_city[$current_user->langcode][0]['value'];
        $stripecustomer->sources->data->address_state   = $current_user->field_basic_cart_plus_state[$current_user->langcode][0]['value'];
        $stripecustomer->sources->data->address_zip     = $current_user->field_basic_cart_plus_s_zip[$current_user->langcode][0]['value'];
        $stripecustomer->sources->data->address_country = $current_user->field_basic_cart_plus_s_country[$current_user->langcode][0]['value'];
        $stripecustomer->save();

        // Registering the new order to the database.
        if (module_exists('basic_cart_plus_order')) {
            basic_cart_plus_save_order($comment);
        }

        basic_cart_plus_mail_order($comment, $total_price);

        basic_cart_plus_empty_cart();
        backdrop_goto('checkout/thank-you');
    }


    catch (\Stripe\Error\Card $e) {
        //Card is declined. $error = $e->getMessage();
        $jbody = $e->getJsonBody();
        $err   = $jbody['error'];
        watchdog('strip_card_error', 'Card error Type: @type, Code: @code, Message: @message', array(
            '@type' => $err['type'],
            '@code' => $err['code'],
            '@message' => $err['message']
        ));
        // backdrop_set_message($err['message']);
        backdrop_set_message($e->getMessage());
        backdrop_set_message("card is declined");
        return;
    }
    catch (\Stripe\InvalidRequestError $e) {
    //Invalid request error.
        $jbody = $e->getJsonBody();
        $err   = $jbody['error'];
        watchdog('strip_card_request_error', 'Invalid Request Param: @param, Message: @message', array(
            //      '@param' => $err['param'],
            '@message' => $err['message']
        ));
        // backdrop_set_message($err['message']);
        backdrop_set_message($e->getMessage());
        backdrop_set_message("Invalid request error");
        return;
    }
    catch (\Stripe\AuthenticationError $e) {
    //Invalid Key request error.
        $jbody = $e->getJsonBody();
        $err   = $jbody['error'];
        watchdog('stripe_invalid_key', 'Key error Type: @type, Message: @message', array(
            '@type' => $err['type'],
            '@message' => $err['message']
        ));
        // backdrop_set_message($err['message']);
        backdrop_set_message($e->getMessage());
        backdrop_set_message("Invalid Key request error with authentication.");
        return;
    }
    catch (\Stripe\Error $e) {
    // General Error.
        $jbody = $e->getJsonBody();
        $err   = $jbody['error'];
        watchdog('stripe_general_error', 'Message: @message', array(
            '@message' => $err['message']
        ));
        backdrop_set_message($e->getMessage());
        backdrop_set_message(t('A error occured, contact the site administrator.'));
        return;
    }
    catch (Exception $e) {
        watchdog('error', 'Message: @message', array(
            '@message' => $err['message']
        ));
        backdrop_set_message($e->getMessage());
        backdrop_set_message(t('A error occured, contact the site administrator.'));
        return;
    }


}
