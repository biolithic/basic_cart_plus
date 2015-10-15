<?php


function basic_cart_plus_admin_settings_config()
{
    $form['basic_cart_plus_whatits'] = array(
        '#type' => 'markup',
        '#title' => t('What is this module?'),
        '#markup' => '<p> This module provides a shopping cart with a built-in checkout/credit card processor to sell simple products.  It is built for modularity, speed and ease of use, and not for ultimate power and options.  You are free to explore something like Drupal Commerce, BigCommerce, Zen Cart, Shopify or Magento instead of making feature requests for bigger uses.</p>'
    );

    $form['basic_cart_plus_warning'] = array(
        '#type' => 'markup',
        '#title' => t('What happens when my customer --buys now--?'),
        '#markup' => '<p> This module does not save ANYTHING about the credit cards or payment to this website.  Do not save anything about the payment for re-occuring billing, record keeping, or anything like that to this site to stay PCI compliant.  This module ships with integrations with Paypal, Braintree or Stripe Payments to limit PCI requirements/responsibilities as much as possible.  This module strives for ease of use and financial safety.  You may view credit card information on each payment processors website admin. <br>&nbsp;<br>Do other shopping carts integrate with every money system possible and keep complete book-keeping on site?  Awesome, use them instead and accept full PCI merchant responsibility. <br>&nbsp;<br>This module requires Javascript to be enabled for your site through out the process to work properly.  Your customers cannot buy products onsite through your website using this module if they have disabled Javascript in their browser. </p>'
    );

    $form['currency'] = array(
        '#title' => t('Currency and price'),
        '#type' => 'fieldset',
        '#description' => t('Please select the currency in which the prices will be calculated.')
    );

    $form['currency']['basic_cart_plus_currency'] = array(
        '#title' => t('Currency'),
        '#type' => 'textfield',
        '#description' => t("Please choose the currency. usd = United States Dollars.  You are responsible for making any other currencies work, including fees and charges.  Here are several documents to help you:  <a target='_blank' href='https://support.stripe.com/questions/which-currencies-does-stripe-support'>https://support.stripe.com/questions/which-currencies-does-stripe-support</a> <a target='_blank' href='https://articles.braintreepayments.com/get-started/payment-methods'>https://articles.braintreepayments.com/get-started/payment-methods</a>  <a target='_blank' href='https://developers.braintreepayments.com/javascript+php/reference/general/currencies'>https://developers.braintreepayments.com/javascript+php/reference/general/currencies</a>"),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_currency')
    );

    $form['currency']['basic_cart_plus_price_format'] = array(
        '#title' => t('Price format'),
        '#type' => 'select',
        '#options' => _basic_cart_plus_price_format(),
        '#description' => t("Please choose the format in which the price will be shown."),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_price_format')
    );

    $form['vat'] = array(
        '#title' => t('VAT'),
        '#type' => 'fieldset'
    );

    $form['vat']['basic_cart_plus_vat_state'] = array(
        '#title' => t('Check if you want to apply the VAT tax on the total amount in the checkout process.'),
        '#type' => 'checkbox',
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_vat_state')
    );

    $form['vat']['basic_cart_plus_vat_value'] = array(
        '#title' => t('VAT value'),
        '#type' => 'textfield',
        '#description' => t("Please enter VAT value."),
        '#field_suffix' => '%',
        '#size' => 10,
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_vat_value')
    );

    $form['basic_cart_plus_redirect_user_after_add_to_cart'] = array(
        '#title' => t('Add to cart redirect'),
        '#type' => 'textfield',
        '#description' => t("Enter the page you wish to redirect the customer to when an item is added to the cart, or &lt;none&gt; for no redirect."),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_redirect_user_after_add_to_cart'),
        '#field_prefix' => url(NULL, array(
            'absolute' => TRUE
        )) . (config_get('system.core', 'clean_url') ? '' : '?q=')
    );

    $form['basic_cart_plus_fields_description'] = array(
        '#type' => 'markup',
        '#title' => t('Which fields do you want to collect?'),
        '#markup' => '<hr /><p>You have the choice to collect the following information from your customers below</p>'
    );

    $form['basic_cart_plus_firstname'] = array(
        '#type' => 'checkbox',
        '#title' => t('First Name?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_firstname'),
        '#description' => t('Check this box to collect first names.  Uncheck to leave the first name box off of the checkout form.')
    );

    $form['basic_cart_plus_lastname'] = array(
        '#type' => 'checkbox',
        '#title' => t('Last Name?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_lastname'),
        '#description' => t('Check this box to collect last names.  Uncheck to leave the last name box off of the checkout form.')
    );

    $form['basic_cart_plus_phone'] = array(
        '#type' => 'checkbox',
        '#title' => t('Phone Number?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_phone'),
        '#description' => t('Check this box to collect phone numbers.  Uncheck to leave the phone number box off of the checkout form.')
    );

    $form['basic_cart_plus_address'] = array(
        '#type' => 'checkbox',
        '#title' => t('Address 1?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_address'),
        '#description' => t('Check this box to collect address 1.  Uncheck to leave the address 1 box off of the checkout form.')
    );

    $form['basic_cart_plus_address2'] = array(
        '#type' => 'checkbox',
        '#title' => t('Address 2?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_address2'),
        '#description' => t('Check this box to collect address 2.  Uncheck to leave the address 2 box off of the checkout form.')
    );

    $form['basic_cart_plus_city'] = array(
        '#type' => 'checkbox',
        '#title' => t('City?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_city'),
        '#description' => t('Check this box to collect city.  Uncheck to leave the city box off of the checkout form.')
    );

    $form['basic_cart_plus_county'] = array(
        '#type' => 'checkbox',
        '#title' => t('County?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_county'),
        '#description' => t('Check this box to collect county.  Uncheck to leave the county box off of the checkout form.')
    );

    $form['basic_cart_plus_state'] = array(
        '#type' => 'checkbox',
        '#title' => t('State?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_state'),
        '#description' => t('Check this box to collect state.  Uncheck to leave the state box off of the checkout form.')
    );

    $form['basic_cart_plus_zip'] = array(
        '#type' => 'checkbox',
        '#title' => t('Zip?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_zip'),
        '#description' => t('Check this box to collect zip codes.  Uncheck to leave the zip code box off of the checkout form.')
    );

    $form['basic_cart_plus_country'] = array(
        '#type' => 'checkbox',
        '#title' => t('Country?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_country'),
        '#description' => t('Check this box to collect countries.  Uncheck to leave the country box off of the checkout form.')
    );

    $form['basic_cart_plus_company'] = array(
        '#type' => 'checkbox',
        '#title' => t('Company?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_company'),
        '#description' => t('Check this box to collect company names.  Uncheck to leave the company box off of the checkout form.')
    );

    $form['basic_cart_plus_website'] = array(
        '#type' => 'checkbox',
        '#title' => t('Website?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_website'),
        '#description' => t('Check this box to collect user websites.  Uncheck to leave the website box off of the checkout form.')
    );

    $form['basic_cart_plus_comment'] = array(
        '#type' => 'checkbox',
        '#title' => t('Comment?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_comment'),
        '#description' => t('Check this box to collect customer comments.  Uncheck to leave the customer comment box off of the checkout form.')
    );

    $form['actions']['#type']  = 'actions';
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save configuration')
    );

    return $form;
}




function basic_cart_plus_admin_checkout_config()
{
    $form['messages'] = array(
        '#title' => t('Email messages'),
        '#type' => 'fieldset',
        '#description' => t('Here you can customize the mails sent to the site administrator and customer, after an order is placed.')
    );

    $form['basic_cart_plus_sendemail'] = array(
        '#type' => 'checkbox',
        '#title' => t('Send emails to admin and customer for each successful transaction?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_sendemail'),
        '#description' => t('Check this box to send emails.  Uncheck to not send emails.')
    );

    $site_mail                                        = config_get('system.core', 'site_mail');
    $form['messages']['basic_cart_plus_admin_emails'] = array(
        '#title' => t('Administrator emails'),
        '#type' => 'textarea',
        '#description' => t('After each placed order, an email with the order details will be sent to all the addresses from the list above.
                          Please add one email address per line.'),
        '#default_value' => $site_mail
    );

    $form['messages']['basic_cart_plus_admin_subject'] = array(
        '#title' => t('Subject'),
        '#type' => 'textfield',
        '#description' => t("Subject field for the administrator's email."),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_admin_subject')
    );

    $form['messages']['basic_cart_plus_admin_message'] = array(
        '#title' => t('Admin email'),
        '#type' => 'textarea',
        '#description' => t('This email will be sent to the site administrator just after an order is placed.
      If you collect these from your customers, you have these available tokens: %CUSTOMER_NAME, %CUSTOMER_EMAIL, %CUSTOMER_PHONE, %CUSTOMER_ADDRESS, %CUSTOMER_ADDRESS2, %CUSTOMER_CITY, %CUSTOMER_STATE, %CUSTOMER_ZIPCODE, %CUSTOMER_COUNTY, %CUSTOMER_COUNTRY, %CUSTOMER_COMPANY, %CUSTOMER_WEBSITE, %CUSTOMER_COMMENT, %ORDER_DETAILS'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_admin_message')
    );

    $form['messages']['basic_cart_plus_send_user_message'] = array(
        '#title' => t('Send an email to the customer after an order is placed'),
        '#type' => 'checkbox',
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_send_user_message')
    );

    $form['messages']['basic_cart_plus_user_subject'] = array(
        '#title' => t('Subject'),
        '#type' => 'textfield',
        '#description' => t("Subject field for the user's email."),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_user_subject')
    );

    $form['messages']['basic_cart_plus_user_message'] = array(
        '#title' => t('User email'),
        '#type' => 'textarea',
        '#description' => t('This email will be sent to the user just after an order is placed. If you collect these from your customers, you have these available tokens: %CUSTOMER_NAME, %CUSTOMER_EMAIL, %CUSTOMER_PHONE, %CUSTOMER_ADDRESS, %CUSTOMER_ADDRESS2, %CUSTOMER_CITY, %CUSTOMER_STATE, %CUSTOMER_ZIPCODE, %CUSTOMER_COUNTY, %CUSTOMER_COUNTRY, %CUSTOMER_COMPANY, %CUSTOMER_WEBSITE, %CUSTOMER_COMMENT, %ORDER_DETAILS'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_user_message')
    );

    $form['thank_you'] = array(
        '#title' => t('Thank you page'),
        '#type' => 'fieldset',
        '#description' => t('Thank you page customization.')
    );

    $form['thank_you']['basic_cart_plus_thank_you_title'] = array(
        '#title' => t('Title'),
        '#type' => 'textfield',
        '#description' => t('Thank you page title.'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_thank_you_title')
    );

    $form['thank_you']['basic_cart_plus_thank_you_message'] = array(
        '#title' => t('Text'),
        '#type' => 'textarea',
        '#description' => t('Thank you page text.'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_thank_you_message')
    );

    $form['actions']['#type']  = 'actions';
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save configuration')
    );

    return $form;
}

function basic_cart_plus_admin_content_config()
{

module_load_include('inc', 'basic_cart_plus', 'basic_cart_plus.cart');
    $node_types = node_type_get_types();
    if (empty($node_types)) {
        return NULL;
    }

    $product_types = config_get('basic_cart_plus.settings', 'basic_cart_plus_content_types');
    $options       = array();
    foreach ($node_types as $node_type => $type) {
        if ($node_type == 'order' && module_exists('basic_cart_plus_order')) {
            continue;
        }
        $options[$node_type] = check_plain($type->name);
    }
    $default_value = array();
    foreach ($product_types as $product_type) {
        if (isset($options[$product_type])) {
            $default_value[$product_type] = $product_type;
        }
    }

    $form['content_type'] = array(
        '#title' => t('Cart Content Type Selection'),
        '#type' => 'fieldset',
        '#description' => t('If you would like to sell products on this website, please select the existing content types for which you wish to have the "Add to cart" button on.  These content types will be your products, and when you uninstall this module, they will go back to being regular nodes. If you make the same content type a membership and a product, weird things might happen you may not be able to fix.')
    );

    $form['content_type']['basic_cart_plus_content_types'] = array(
        '#title' => t('Cart Content types'),
        '#type' => 'checkboxes',
        '#options' => $options,
        '#default_value' => $default_value
    );

    $form['content_type']['basic_cart_plus_price'] = array(
        '#type' => 'markup',
        '#title' => t('Price of product?'),
        '#markup' => '<p>' . t('You can set the price of each product at its own node edit screen, not here.') . '</p>'
    );

    $form['paywall_members'] = array(
        '#title' => t('Membership Content Type Selection'),
        '#type' => 'fieldset',
        '#description' => t('If you would like to sell flat memberships of content on this website, please select the existing content types for which you wish charge membership to view.  These content types will be your premium content, and when you uninstall this module, they will go back to being regular open nodes.  If you make the same content type a membership and a product, weird things might happen you may not be able to fix.')
    );

    foreach (array_keys(node_type_get_names()) as $term) {
        if ($term == 'order' && module_exists('basic_cart_plus_order')) {
            continue;
        }
        $form['paywall_members']["paywall_member_$term"] = array(
            '#type' => 'checkbox',
            '#title' => t("Do you want to charge for $term ?"),
            '#default_value' =>  config_get('basic_cart_plus.settings', "paywall_member_$term"),
            '#description' => t("Uncheck to allow all users to view nodes of content type $term .  Check to limit access to one-time paying members.")
        );
    }

    $form['paywall_members']['basic_cart_plus_paywall_price'] = array(
        '#type' => 'textfield',
        '#title' => t('Flat price of membership'),
        '#default_value' =>  config_get('basic_cart_plus.settings', 'basic_cart_plus_paywall_price'),
        '#description' => t('Flat price of membership if you choose to charge once for access.'),
        '#required' => TRUE
    );

    $form['paywall_subscription'] = array(
        '#title' => t('Subscription Content Type Selection'),
        '#type' => 'fieldset',
        '#description' => t('If you would like to sell subscription of content on this website, please select the existing content types for which you wish charge to view.  These content types will be your premium content, and when you uninstall this module, they will go back to being regular open nodes.  If you make the same content type a membership and a product, weird things might happen you may not be able to fix.<br>&nbsp;<br>You get three out-of-the-box subscription levels that you can use: bronze, silver and gold.  You may choose as many as needed and publically call them what you want -- simpler and less may be better to maintain.<br>&nbsp;<br>It is not wise to check the box for a content type (Post) for more than one level -- ie...bronze and silver.  A customer who buys the silver subscription should not have to also buy the bronze level to view a post.  Subscriptions are not/should not be set up until they have been financially set up and verified as plans in either Braintree or Stripe developer dashboards.  Below are the last steps afterwards.')
    );

    $form['paywall_subscription']['basic_cart_plus_subscription_bronze'] = array(
        '#type' => 'markup',
        '#title' => t('Bronze'),
        '#markup' => '<p>' . t('- Bronze -') . '</p>'
    );

    foreach (array_keys(node_type_get_names()) as $term) {
        if ($term == 'order' && module_exists('basic_cart_plus_order')) {
            continue;
        }
        $form['paywall_subscription']["paywall_bronze_$term"] = array(
            '#type' => 'checkbox',
            '#title' => t("Do you want to charge the bronze level for $term ?"),
            '#default_value' =>  config_get('basic_cart_plus.settings', "paywall_bronze_$term"),
            '#description' => t("Uncheck to allow all users to view nodes of content type $term .  Check to limit access to bronze subscription paying members.")
        );
    }

    $form['paywall_subscription']['basic_cart_plus_subscription_silver'] = array(
        '#type' => 'markup',
        '#title' => t('Silver'),
        '#markup' => '<p>' . t('- Silver -') . '</p>'
    );

    foreach (array_keys(node_type_get_names()) as $term) {
        if ($term == 'order' && module_exists('basic_cart_plus_order')) {
            continue;
        }
        $form['paywall_subscription']["paywall_silver_$term"] = array(
            '#type' => 'checkbox',
            '#title' => t("Do you want to charge the silver level for $term ?"),
            '#default_value' =>  config_get('basic_cart_plus.settings', "paywall_silver_$term"),
            '#description' => t("Uncheck to allow all users to view nodes of content type $term .  Check to limit access to silver subscription paying members.")
        );
    }

    $form['paywall_subscription']['basic_cart_plus_subscription_gold'] = array(
        '#type' => 'markup',
        '#title' => t('Gold'),
        '#markup' => '<p>' . t('- Gold -') . '</p>'
    );

    foreach (array_keys(node_type_get_names()) as $term) {
        if ($term == 'order' && module_exists('basic_cart_plus_order')) {
            continue;
        }
        $form['paywall_subscription']["paywall_gold_$term"] = array(
            '#type' => 'checkbox',
            '#title' => t("Do you want to charge the gold level for $term ?"),
            '#default_value' =>  config_get('basic_cart_plus.settings', "paywall_gold_$term"),
            '#description' => t("Uncheck to allow all users to view nodes of content type $term .  Check to limit access to gold subscription paying members.")
        );
    }

    $form['paywall_subscription']['basic_cart_plus_subscription_price'] = array(
        '#type' => 'markup',
        '#title' => t('Price of subscription?'),
        '#markup' => '<p>' . t('You can set the price of each subscription when creating the plan  elsewhere, not here.') . '</p>'
    );

    $form['actions']['#type']  = 'actions';
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save configuration')
    );

return $form;

}


function basic_cart_plus_admin_payment_config()
{
$payments = array(0 => 'none', 1 => 'stripe', 2 => 'braintree');
    $form['basic_cart_plus_which_payment'] = array (
    '#title' => t('Check if you want to use no payment processor, Stripe Payments, or Braintree Payments.'),
    '#type' => 'radios',
    '#options' => $payments,
    '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_which_payment')
    );

    $form['braintree_section'] = array(
        '#title' => t('Braintree Processor Settings'),
        '#type' => 'fieldset',
        '#description' => t('If you plan on using Braintree Payments, enter your information below')
    );

    $form['braintree_section']['basic_cart_plus_braintree_wherefrom'] = array(
        '#type' => 'markup',
        '#title' => t('Where to get the Braintree TEST account information for this page from?'),
        '#markup' => '<p>' . l(t('Sign up for a Braintree Payments test account:'), 'https://www.braintreepayments.com/get-started') . '</p>'
    );

    $form['braintree_section']['basic_cart_plus_braintree_wherefrom2'] = array(
        '#type' => 'markup',
        '#title' => t('Where to get the Braintree LIVE account information for this page from?'),
        '#markup' => '<p>' . l(t('Sign up for a Braintree Payments live account:'), 'https://braintreepayments.com') . '</p>'
    );

    $form['braintree_section']['basic_cart_plus_braintree_environment'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree environment setting'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_environment'),
        '#description' => t('Braintree environment setting'),
        '#required' => TRUE
    );

    $form['braintree_section']['basic_cart_plus_braintree_merchantId_testkey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree Test Merchant Id'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_merchantId_testkey'),
        '#description' => t('Braintree Test Merchant Id'),
        '#required' => TRUE
    );

    $form['braintree_section']['basic_cart_plus_braintree_merchantId_livekey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree Live Merchant Id'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_merchantId_livekey'),
        '#description' => t('Braintree Live Merchant Id'),
        '#required' => TRUE
    );

    $form['braintree_section']['basic_cart_plus_braintree_publishable_testkey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree test public API Key'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_publishable_testkey'),
        '#description' => t('Braintree test public API Key'),
        '#required' => TRUE
    );

    $form['braintree_section']['basic_cart_plus_braintree_publishable_livekey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree live public API Key'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_publishable_livekey'),
        '#description' => t('Braintree live public API Key'),
        '#required' => TRUE
    );

    $form['braintree_section']['basic_cart_plus_braintree_secret_testkey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree test secret API Key'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_secret_testkey'),
        '#description' => t('Braintree test secret API Key'),
        '#required' => TRUE
    );

    $form['braintree_section']['basic_cart_plus_braintree_secret_livekey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree live secret API Key'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_secret_livekey'),
        '#description' => t('Braintree live secret API Key'),
        '#required' => TRUE
    );

    $form['braintree_section']['basic_cart_plus_braintree_gonelive'] = array(
        '#type' => 'checkbox',
        '#title' => t('Live?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_gonelive'),
        '#description' => t('Check this box to use your live key.  Uncheck to enter demo mode and use your test key')
    );

    $form['braintree_section']['basic_cart_plus_braintree_use_dropin'] = array(
        '#type' => 'checkbox',
        '#title' => t('Drop in UI?'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_braintree_use_dropin'),
        '#description' => t('Check this box to use the new magic Javascript UI and PayPal.  Uncheck to use the traditional credit card form - recommended')
    );

    $form['stripe_section'] = array(
        '#title' => t('Stripe Processor Settings'),
        '#type' => 'fieldset',
        '#description' => t('If you plan on using Stripe Payments, enter your information below')
    );

    $form['stripe_section']['basic_cart_plus_stripe_wherefrom'] = array(
        '#type' => 'markup',
        '#title' => t('Where to get the Stripe TEST account information for this page from?'),
        '#markup' => '<p>' . l(t('Sign up for a Stripe test account:'), 'https://stripe.com') . '</p>'
    );

    $form['stripe_section']['basic_cart_plus_stripe_wherefrom2'] = array(
        '#type' => 'markup',
        '#title' => t('Where to get the Stripe TEST account information for this page from?'),
        '#markup' => '<p>' . l(t('Sign up for a Stripe live account:'), 'https://stripe.com') . '</p>'
    );

    $form['stripe_section']['basic_cart_plus_stripe_publishable_testkey'] = array(
        '#type' => 'textfield',
        '#title' => t('Stripe test public API Key'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_publishable_testkey'),
        '#description' => t('Stripe test public API Key'),
        '#required' => TRUE
    );

    $form['stripe_section']['basic_cart_plus_stripe_publishable_livekey'] = array(
        '#type' => 'textfield',
        '#title' => t('Stripe live public API Key'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_publishable_livekey'),
        '#description' => t('Stripe live public API Key'),
        '#required' => TRUE
    );

    $form['stripe_section']['basic_cart_plus_stripe_secret_testkey'] = array(
        '#type' => 'textfield',
        '#title' => t('Stripe test secret API Key'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_secret_testkey'),
        '#description' => t('Stripe test secret API Key'),
        '#required' => TRUE
    );

    $form['stripe_section']['basic_cart_plus_stripe_secret_livekey'] = array(
        '#type' => 'textfield',
        '#title' => t('Stripe live secret API Key'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_secret_livekey'),
        '#description' => t('Stripe live secret API Key'),
        '#required' => TRUE
    );

    $form['stripe_section']['basic_cart_plus_stripe_setApiVersion'] = array(
        '#type' => 'textfield',
        '#title' => t('Stripe API version'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_setApiVersion'),
        '#description' => t('Stripe API version if you need to change it'),
        '#required' => TRUE
    );

    $form['stripe_section']['basic_cart_plus_stripe_gonelive'] = array(
        '#type' => 'checkbox',
        '#title' => t('Live'),
        '#default_value' => config_get('basic_cart_plus.settings', 'basic_cart_plus_stripe_gonelive'),
        '#description' => t('Check this box to use your live key.  Uncheck to enter demo mode and use your test key')
    );

$form['actions']['#type']  = 'actions';
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save configuration')
    );

return $form;

}


// SUBMIT FUNCTIONS BELOW HERE

function basic_cart_plus_admin_settings_config_validate($form, &$form_state)
{
    $vat_is_enabled = (int) $form_state['values']['basic_cart_plus_vat_state'];
    if (!empty($vat_is_enabled) && $vat_is_enabled) {
        $vat_value = (float) $form_state['values']['basic_cart_plus_vat_value'];
        if ($vat_value <= 0) {
            form_set_error('basic_cart_plus_vat_value', t('Please enter a valid figure for the VAT.'));
        }
    }
}


function basic_cart_plus_admin_settings_config_submit($form, &$form_state)
{
    unset($form_state['values']['form_id'], $form_state['values']['form_token'], $form_state['values']['form_build_id'], $form_state['values']['op']);

    config_set('basic_cart_plus.settings', 'basic_cart_plus_content_types', $product_types);
    if (empty($form_state['values']['basic_cart_plus_vat_state'])) {
        $form_state['values']['basic_cart_plus_vat_value'] = '';
        if (module_exists('basic_cart_plus_order')) {
            $instance = field_info_instance('node', 'vat', 'order');
            if (!empty($instance)) {
                field_delete_instance($instance);
            }
        }
    } else {
        if (module_exists('basic_cart_plus_order')) {
            $field = field_info_field('vat');
            if (empty($field)) {
                $field = array(
                    'field_name' => 'vat',
                    'type' => 'number_decimal',
                    'entity_types' => array(
                        'node'
                    )
                );
                field_create_field($field);

                $instance = field_info_instance('node', 'vat', 'order');
                if (empty($instance)) {
                    $instance = array(
                        'field_name' => 'vat',
                        'label' => t('VAT'),
                        'description' => t('The VAT tax.'),
                        'entity_type' => 'node',
                        'bundle' => 'order'
                    );
                    field_create_instance($instance);
                }
            }
        }
    }

    foreach ($form_state['values'] as $key => $value) {
        if (is_array($value) && isset($form_state['values']['array_filter'])) {
            $value = array_keys(array_filter($value));
        }
        config_set('basic_cart_plus.settings', $key, $value);
    }

    backdrop_set_message(t('The configuration options have been saved.'));
}


function basic_cart_plus_admin_content_config_submit($form, &$form_state)
{

$product_types = array();
    $content_types = $form_state['values']['basic_cart_plus_content_types'];
    unset($form_state['values']['basic_cart_plus_content_types']);

    if (!empty($content_types) && is_array($content_types)) {
        $field = field_info_field('price');
        if (empty($field)) {
            $field = array(
                'field_name' => 'price',
                'type' => 'number_decimal',
                'entity_types' => array(
                    'node'
                )
            );
            field_create_field($field);
        }
        foreach ($content_types as $type => $checked) {
            if ($checked) {
                $product_types[$type] = $type;
                $instance             = field_info_instance('node', 'price', $type);
                if (empty($instance)) {
                    $instance = array(
                        'field_name' => 'price',
                        'label' => t('Price'),
                        'description' => t('Please enter the price for this item.'),
                        'entity_type' => 'node',
                        'bundle' => $type
                    );
                    field_create_instance($instance);
                }
            } else {
                $instance = field_info_instance('node', 'price', $type);
                if (!empty($instance)) {
                    field_delete_instance($instance);
                }
            }
        }
    }

foreach ($form_state['values'] as $key => $value) {
        if (is_array($value) && isset($form_state['values']['array_filter'])) {
            $value = array_keys(array_filter($value));
        }
        config_set('basic_cart_plus.settings', $key, $value);
    }

backdrop_set_message(t('The configuration options have been saved.'));

}

function basic_cart_plus_admin_payment_config_submit($form, &$form_state)
{
foreach ($form_state['values'] as $key => $value) {
        if (is_array($value) && isset($form_state['values']['array_filter'])) {
            $value = array_keys(array_filter($value));
        }
        config_set('basic_cart_plus.settings', $key, $value);
    }

    backdrop_set_message(t('The configuration options have been saved.'));
}

function basic_cart_plus_admin_checkout_config_submit($form, &$form_state)
{
    unset($form_state['values']['form_id'], $form_state['values']['form_token'], $form_state['values']['form_build_id'], $form_state['values']['op']);
    foreach ($form_state['values'] as $key => $value) {
        if (is_array($value) && isset($form_state['values']['array_filter'])) {
            $value = array_keys(array_filter($value));
        }
        config_set('basic_cart_plus.settings', $key, $value);
    }

    backdrop_set_message(t('The configuration options have been saved.'));
}
