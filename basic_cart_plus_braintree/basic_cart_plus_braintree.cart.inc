<?php

/**
 * @file
 * Basic Cart Plus Braintree shopping cart implementation functions.
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
function basic_cart_plus_braintree_form_payment($form, $form_state)
{
    $dropin = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_use_dropin');

    $form['payment'] = array(
        '#type' => 'fieldset',
        '#title' => t('Payment Details'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE
    );

    $form['payment']['braintreetoken'] = array(
        '#type' => 'hidden',
        '#title' => t('braintreetoken')
    );

    // START IF HERE
    if ($dropin > 0) {

        $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_publishable_testkey');
        $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_publishable_livekey');
        $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_gonelive');

        if ($three > 0) {
            $basic_cart_plus_braintree_publishable_key = $two;
        } else {
            $basic_cart_plus_braintree_publishable_key = $one;
        }

        $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_secret_testkey');
        $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_secret_livekey');
        $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_gonelive');

        if ($three > 0) {
            $basic_cart_plus_braintree_secret_key = $two;
        } else {
            $basic_cart_plus_braintree_secret_key = $one;
        }

        $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_merchantId_testkey');
        $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_merchantId_livekey');
        $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_gonelive');

        if ($three > 0) {
            $basic_cart_plus_braintree_merchantId = $two;
        } else {
            $basic_cart_plus_braintree_merchantId = $one;
        }

        //  empty($basic_cart_braintree_secret_key) ? NULL : basic_cart_braintree::setApiKey($basic_cart_braintree_secret_key);
        Braintree_Configuration::environment(config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_environment'));
        Braintree_Configuration::merchantId($basic_cart_plus_braintree_merchantId);
        Braintree_Configuration::publicKey($basic_cart_plus_braintree_publishable_key);
        Braintree_Configuration::privateKey($basic_cart_plus_braintree_secret_key);
        $btClientToken = Braintree_ClientToken::generate();

        $form['braintree_placeholder'] = array(
            '#markup' => '<div id="braintree"></div>',
            '#prefix' => '<p>',
            '#suffix' => '</p>'
        );

        $form['braintree_checkout_submit'] = array(
            '#type' => 'submit',
            '#value' => t('Pay')
        );

        $form['submit']['#validate'] = array(
            'basic_cart_plus_create_form_validate'
        );

        backdrop_add_js("https://js.braintreegateway.com/v2/braintree.js", array(
            'type' => 'file',
            'scope' => 'footer',
            'every_page' => TRUE,
            'preprocess' => FALSE
        ));

        backdrop_add_js("jQuery(document).ready(function () {    alert('hello'); braintree.setup('" . $btClientToken . "', 'dropin', { container: 'braintree' })      });", 'inline');

    }
    // END IF HERE

    else {

        backdrop_add_js('https://js.braintreegateway.com/v1/braintree.js', 'external');
        backdrop_add_js(backdrop_get_path('module', 'basic_cart_plus_braintree') . '/js/basic_cart_plus_braintree.js');

        $form['payment']['card'] = array(
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
        $form['payment']['cvc']  = array(
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

        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Submit'),
            '#submit' => array(
                'basic_cart_plus_braintree_form_callback'
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
                'basic_cart_plus_form_back'
            ),
            '#limit_validation_errors' => array()
        );

        $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_publishable_testkey');
        $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_publishable_livekey');
        $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_gonelive');

        if ($three > 0) {
            $publishable_api_key = $two;
        } else {
            $publishable_api_key = $one;
        }

        $form['basic_cart_plus_braintree_publishable_key'] = array(
            '#type' => 'hidden',
            '#value' => $publishable_api_key,
            '#attributes' => array(
                'id' => array(
                    'basic_cart_plus_braintree_publishable_key'
                )
            )
        );

        $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_publishable_testkey');
        $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_publishable_livekey');
        $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_gonelive');
        if ($three > 0) {
            $basic_cart_plus_braintree_publishable_key = $two;
        } else {
            $basic_cart_plus_braintree_publishable_key = $one;
        }

        $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_secret_testkey');
        $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_secret_livekey');
        $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_gonelive');

        if ($three > 0) {
            $basic_cart_plus_braintree_secret_key = $two;
        } else {
            $basic_cart_plus_braintree_secret_key = $one;
        }

    }
    // END IF ELSE HERE

    $form['html'] = array(
        '#type' => 'markup',
        '#markup' => '<br />&nbsp;<br /><a href="https://www.braintreegateway.com/merchants/n3cdm2sgkjqt5c4g/verified" target="_blank"><img src="https://braintree-badges.s3.amazonaws.com/05.png" border="0"/></a><br />For more information, see the <a href="https://www.braintreepayments.com/developers/security">Security Docs</a><br />&nbsp;<br />'
    );

    return $form;
}


function basic_cart_plus_create_form_validate($form, $form_state)
{
    // Make sure we have a valid nonce (sale token) returned from Braintree.
    $nonce = basic_cart_plus_braintree_dropin_get_nonce();

    if (empty($nonce) && empty($form_state["values"]["card"])) {
        form_set_error('braintree_dropin', t('We were unable to charge your card at this time.'));
        return FALSE;
    }
    basic_cart_plus_braintree_form_callback($form, $form_state, $nonce);
}

function basic_cart_plus_braintree_dropin_get_nonce()
{
    return !empty($_POST['payment_method_nonce']) ? check_plain($_POST['payment_method_nonce']) : NULL;
}

/**
 * Process Braintree Charge and save user data.
 *
 * @param array $form
 * @param array $form_state
 */
function basic_cart_plus_braintree_form_callback($form, $form_state, $nonce)
{
    $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_publishable_testkey');
    $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_publishable_livekey');
    $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_gonelive');

    if ($three > 0) {
        $basic_cart_plus_braintree_publishable_key = $two;
    } else {
        $basic_cart_plus_braintree_publishable_key = $one;
    }

    $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_secret_testkey');
    $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_secret_livekey');
    $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_gonelive');

    if ($three > 0) {
        $basic_cart_plus_braintree_secret_key = $two;
    } else {
        $basic_cart_plus_braintree_secret_key = $one;
    }

    $one   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_merchantId_testkey');
    $two   = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_merchantId_livekey');
    $three = config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_gonelive');

    if ($three > 0) {
        $basic_cart_plus_braintree_merchantId = $two;
    } else {
        $basic_cart_plus_braintree_merchantId = $one;
    }

    //  empty($basic_cart_braintree_secret_key) ? NULL : basic_cart_braintree::setApiKey($basic_cart_braintree_secret_key);
    Braintree_Configuration::environment(config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_environment'));
    Braintree_Configuration::merchantId($basic_cart_plus_braintree_merchantId);
    Braintree_Configuration::publicKey($basic_cart_plus_braintree_publishable_key);
    Braintree_Configuration::privateKey($basic_cart_plus_braintree_secret_key);

    // Assign billing data for easier referencing.
    $billing_values = $form_state['page_values']['1'];
    $comment = basic_cart_plus_sanitize_data($billing_values);

    $order_details = '';
    $cart          = basic_cart_plus_get_cart();
    $price         = basic_cart_plus_get_total_price();
    $total_price   = basic_cart_plus_price_format($price->total);

    $vat_is_enabled = (int) config_get('basic_cart_plus.settings', 'basic_cart_plus_vat_state');
    if (!empty($vat_is_enabled) && $vat_is_enabled) {
        $vat = basic_cart_plus_price_format($price->vat);
    }

    // Building the order details.
    $i = 0;
    // Cart elements.
    foreach ($cart as $cid => $value) {
        if (is_numeric($cid)) {
            $node       = node_load($cart[$value]);

global $user;
foreach (array_keys(node_type_get_names()) as $term) {
$testvariable = "paywall_member_" . $term;
$isenabled = config_get('basic_cart_plus.settings', $testvariable);
if ($node->type == $term && (arg(1) != $node->uid) && ($isenabled > 0) && !in_array("paywall_member", $user->roles)) {
$basic_cart_plus_paywall_price = config_get('basic_cart_plus.settings', "basic_cart_plus_paywall_price");
$order_details .= ++$i . ". " . $node->title . "\t 1 x " . $unit_price . "\n";
$sitemembership = "yes";
}
}

if (!isset($sitemembership))
{
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

// COMMENT HERE

global $user;
$current_user = user_load($user->uid);
if (isset($current_user->field_basic_cart_plus_b_id[$current_user->langcode]))
{
$braintreeid = $current_user->field_basic_cart_plus_b_id[$current_user->langcode][0]['value'];
}
else
{
$braintreeid = "123";
}


try {
$braintreecustomer = Braintree_Customer::find($braintreeid);
$braintreecustomerid = $customer->id;

if (isset($nonce)) {
        $result = Braintree_Transaction::sale(array(
            // 'orderId' => $orderId,
            'amount' => $total_price,
            'paymentMethodNonce' => $nonce,
            'options' => array(
                'submitForSettlement' => false,
                'storeInVaultOnSuccess' => true
            )
        ));
    } else {
        $result = Braintree_Transaction::sale(array(
            'amount' => $total_price,
            'creditCard' => array(
                'number' => $form_state['values']['card'],
                'expirationMonth' => $form_state['values']['month'],
                'expirationYear' => $form_state['values']['year']
            )
        ));
    }

if ($result->success) {

$uid = $user->uid; // User ID of user that you want to add role to.
user_multiple_role_edit(array($uid), 'add_role', "paywall_member");

$updateResult = Braintree_Customer::update(
    $braintreecustomerid,
    [
      'firstName' => $current_user->field_basic_cart_plus_firstname[$current_user->langcode]['0']['value'],
      'lastName' => $current_user->field_basic_cart_plus_lastname[$current_user->langcode]['0']['value'],
      'phone' => $current_user->field_basic_cart_plus_phone[$current_user->langcode]['0']['value'],
      'website' => $current_user->field_basic_cart_plus_website[$current_user->langcode]['0']['value'],
      'billingAddress' => [
            'firstName' => $current_user->field_basic_cart_plus_firstname[$current_user->langcode]['0']['value'],
            'lastName' => $current_user->field_basic_cart_plus_lastname[$current_user->langcode]['0']['value'],
            'streetAddress' => $current_user->field_basic_cart_plus_address[$current_user->langcode]['0']['value'],
            'extendedAddress' => $current_user->field_basic_cart_plus_address2[$current_user->langcode]['0']['value'],
            'locality' => $current_user->field_basic_cart_plus_city[$current_user->langcode]['0']['value'],
            'region' => $current_user->field_basic_cart_plus_state[$current_user->langcode]['0']['value'],
            'postalCode' => $current_user->field_basic_cart_plus_zip[$current_user->langcode]['0']['value'],
            'countryCodeAlpha3' => 'USA',
            'company' => $current_user->field_basic_cart_plus_company[$current_user->langcode]['0']['value'],
            'options' => [
                'updateExisting' => true
            ]
        ]

    ]
);

if ($updateResult->success)
{
backdrop_set_message("Order received and processed.  Thank you!");
}

}
else {
foreach($result->errors->deepAll() AS $error) {
backdrop_set_message($error->code . ": " . $error->message . "\n");
}
}

}
catch(Exception $e) {

backdrop_set_message("Customer not found", "error");

if (isset($nonce)) {
        $result = Braintree_Transaction::sale(array(
            // 'orderId' => $orderId,
            'amount' => $total_price,
            'paymentMethodNonce' => $nonce,
            'options' => array(
                'submitForSettlement' => false,
                'storeInVaultOnSuccess' => true
            )
        ));
    } else {
        $result = Braintree_Transaction::sale(array(
            'amount' => $total_price,
            'creditCard' => array(
                'number' => $form_state['values']['card'],
                'expirationMonth' => $form_state['values']['month'],
                'expirationYear' => $form_state['values']['year']
            )
        ));
    }

if ($result->success){

if (isset($nonce)) {

$createresult = Braintree_Customer::create([
      'paymentMethodNonce' => $nonce,
      'firstName' => $current_user->field_basic_cart_plus_firstname[$current_user->langcode]['0']['value'],
      'lastName' => $current_user->field_basic_cart_plus_lastname[$current_user->langcode]['0']['value'],
      'phone' => $current_user->field_basic_cart_plus_phone[$current_user->langcode]['0']['value'],
      'website' => $current_user->field_basic_cart_plus_website[$current_user->langcode]['0']['value'],
      'creditCard' => [
      'options' => [
                'makeDefault' => true
            ],
      'billingAddress' => [
            'firstName' => $current_user->field_basic_cart_plus_firstname[$current_user->langcode]['0']['value'],
            'lastName' => $current_user->field_basic_cart_plus_lastname[$current_user->langcode]['0']['value'],
            'streetAddress' => $current_user->field_basic_cart_plus_address[$current_user->langcode]['0']['value'],
            'extendedAddress' => $current_user->field_basic_cart_plus_address2[$current_user->langcode]['0']['value'],
            'locality' => $current_user->field_basic_cart_plus_city[$current_user->langcode]['0']['value'],
            'region' => $current_user->field_basic_cart_plus_state[$current_user->langcode]['0']['value'],
            'postalCode' => $current_user->field_basic_cart_plus_zip[$current_user->langcode]['0']['value'],
            'countryCodeAlpha3' => 'USA',
            'company' => $current_user->field_basic_cart_plus_company[$current_user->langcode]['0']['value']
        ]
        ]
    ]);

if ($createresult->success) {

}

} else {

$createresult = Braintree_Customer::create([
      'firstName' => $current_user->field_basic_cart_plus_firstname[$current_user->langcode]['0']['value'],
      'lastName' => $current_user->field_basic_cart_plus_lastname[$current_user->langcode]['0']['value'],
      'phone' => $current_user->field_basic_cart_plus_phone[$current_user->langcode]['0']['value'],
      'website' => $current_user->field_basic_cart_plus_website[$current_user->langcode]['0']['value'],
      'creditCard' => [
      'number' => $form_state['values']['card'],
      'expirationMonth' => $form_state['values']['month'],
      'expirationYear' => $form_state['values']['year'],
      'options' => [
                'makeDefault' => true
            ],
      'billingAddress' => [
            'firstName' => $current_user->field_basic_cart_plus_firstname[$current_user->langcode]['0']['value'],
            'lastName' => $current_user->field_basic_cart_plus_lastname[$current_user->langcode]['0']['value'],
            'streetAddress' => $current_user->field_basic_cart_plus_address[$current_user->langcode]['0']['value'],
            'extendedAddress' => $current_user->field_basic_cart_plus_address2[$current_user->langcode]['0']['value'],
            'locality' => $current_user->field_basic_cart_plus_city[$current_user->langcode]['0']['value'],
            'region' => $current_user->field_basic_cart_plus_state[$current_user->langcode]['0']['value'],
            'postalCode' => $current_user->field_basic_cart_plus_zip[$current_user->langcode]['0']['value'],
            'countryCodeAlpha3' => 'USA',
            'company' => $current_user->field_basic_cart_plus_company[$current_user->langcode]['0']['value']
        ]
        ]
    ]);

if ($createresult->success) {

}

}




}
else {
foreach($result->errors->deepAll() AS $error) {
backdrop_set_message($error->code . ": " . $error->message . "\n");
}



}
}
}
