<?php

/**
 * @file
 * Basic cart admin settings forms.
 */

/**
 * Callback for the admin configuration page.
 */
function basic_cart_braintree_admin_content_type() {
  module_load_include('inc', 'basic_cart_braintree', 'basic_cart_braintree.cart');
  $node_types = node_type_get_types();
  if (empty($node_types)) {
    return NULL;
  }
  

  $options = array();
  foreach ($node_types as $node_type => $type) {
    if ($node_type == 'order' && module_exists('basic_cart_braintree_order')) {
      continue;
    }
    $options[$node_type] = check_plain($type->name);
  }
  $default_value = array();
  foreach (basic_cart_braintree_product_types() as $product_type){
    if (isset($options[$product_type])){
      $default_value[$product_type] = $product_type;
    }
  }

  $form['content_type'] = array(
    '#title' => t('Content type selection'),
    '#type' => 'fieldset',
    '#description' => t('Please select the content types for which you wish to have the "Add to cart" option.'),
  );

  $form['content_type']['basic_cart_braintree_content_types'] = array(
    '#title' => t('Content types'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $default_value,
  );

  $form['currency'] = array(
    '#title' => t('Currency and price'),
    '#type' => 'fieldset',
    '#description' => t('Please select the currency in which the prices will be calculated.'),
  );

  $form['currency']['basic_cart_braintree_currency'] = array(
    '#title' => t('Currency'),
    '#type' => 'textfield',
    '#description' => t("Please choose the currency."),
    '#default_value' => variable_get('basic_cart_braintree_currency'),
  );

  $form['currency']['basic_cart_braintree_price_format'] = array(
    '#title' => t('Price format'),
    '#type' => 'select',
    '#options' => _basic_cart_braintree_price_format(),
    '#description' => t("Please choose the format in which the price will be shown."),
    '#default_value' => variable_get('basic_cart_braintree_price_format'),
  );
  
  $form['vat'] = array(
    '#title' => t('VAT'),
    '#type' => 'fieldset',
  );

  $form['vat']['basic_cart_braintree_vat_state'] = array(
    '#title' => t('Check if you want to apply the VAT tax on the total amount in the checkout process.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('basic_cart_braintree_vat_state', FALSE),
  );

  $form['vat']['basic_cart_braintree_vat_value'] = array(
    '#title' => t('VAT value'),
    '#type' => 'textfield',
    '#description' => t("Please enter VAT value."),
    '#field_suffix' => '%',
    '#size' => 10,
    '#default_value' => variable_get('basic_cart_braintree_vat_value', ''),
  );
  
  $form['redirect'] = array(
    '#title' => t('Redirect user after adding an item to the shopping cart'),
    '#type' => 'fieldset',
  );
  
  $form['redirect']['basic_cart_braintree_redirect_user_after_add_to_cart'] = array(
    '#title' => t('Add to cart redirect'),
    '#type' => 'textfield',
    '#description' => t("Enter the page you wish to redirect the customer to when an item is added to the cart, or &lt;none&gt; for no redirect."),
    '#default_value' => variable_get('basic_cart_braintree_redirect_user_after_add_to_cart'),
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );
  
  $form['basic_cart_braintree_readme'] = array(
        '#type' => 'markup',
        '#title' => t('Read me instructions'),
        '#markup' => '<p>' . l(t('Read me instructions'), 'admin/config/services/basic_cart_braintree/readme') . '</p>'
    );
    
    $form['basic_cart_braintree_warning'] = array(
        '#type' => 'markup',
        '#title' => t('What happens when my customer --buys now--?'),
        '#markup' => '<p> This module does not save ANYTHING about the credit cards or payment to this website.  The user may type it into the form on this website which gets sent immediately only to Braintree encrypted and you will only get a pass/fail message back.  Do not save anything about the payment for re-occuring billing, record keeping, or anything like that to this site.  You may view any details about each transaction at your Braintree account page. <br>&nbsp;<br>This module requires Javascript to be enabled for your site through out the process to work properly.  Your customers cannot buy products through your website using this module if they have disabled Javascript in their browser. </p>'
    );
    
    $form['basic_cart_braintree_whatits'] = array(
        '#type' => 'markup',
        '#title' => t('What is this module?'),
        '#markup' => '<p> basic_cart_braintree produces a shopping cart with a built-in checkout/credit card processor to sell single-file digital content like e-books.  Drupal Commerce is recommended, but if that suite is too large for your needs, this is a simple payment solution.</p>'
    );
    
    $form['basic_cart_braintree_wherefrom'] = array(
        '#type' => 'markup',
        '#title' => t('Where to get the TEST information for this page from?'),
        '#markup' => '<p>' . l(t('Sign up for a Braintree Payments test account:'), 'https://www.braintreepayments.com/get-started') . '</p>'
    );
    
    $form['basic_cart_braintree_wherefrom2'] = array(
        '#type' => 'markup',
        '#title' => t('Where to get the LIVE information for this page from?'),
        '#markup' => '<p>' . l(t('Sign up for a Braintree Payments live account:'), 'https://braintreepayments.com') . '</p>'
    );
    
    $form['basic_cart_braintree_environment'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree environment setting'),
        '#default_value' => variable_get('basic_cart_braintree_environment', ""),
        '#description' => t('Braintree environment setting'),
        '#required' => TRUE
    );
    
    $form['basic_cart_braintree_merchantId_testkey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree Test Merchant Id'),
        '#default_value' => variable_get('basic_cart_braintree_merchantId_testkey', ""),
        '#description' => t('Braintree Test Merchant Id'),
        '#required' => TRUE
    );
    
    $form['basic_cart_braintree_merchantId_livekey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree Live Merchant Id'),
        '#default_value' => variable_get('basic_cart_braintree_merchantId_livekey', ""),
        '#description' => t('Braintree Live Merchant Id'),
        '#required' => TRUE
    );
    
    $form['basic_cart_braintree_publishable_testkey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree test public API Key'),
        '#default_value' => variable_get('basic_cart_braintree_publishable_testkey', ""),
        '#description' => t('Braintree test public API Key'),
        '#required' => TRUE
    );
    
    $form['basic_cart_braintree_publishable_livekey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree live public API Key'),
        '#default_value' => variable_get('basic_cart_braintree_publishable_livekey', ""),
        '#description' => t('Braintree live public API Key'),
        '#required' => TRUE
    );
    
    $form['basic_cart_braintree_secret_testkey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree test secret API Key'),
        '#default_value' => variable_get('basic_cart_braintree_secret_testkey', ""),
        '#description' => t('Braintree test secret API Key'),
        '#required' => TRUE
    );
    
    $form['basic_cart_braintree_secret_livekey'] = array(
        '#type' => 'textfield',
        '#title' => t('Braintree live secret API Key'),
        '#default_value' => variable_get('basic_cart_braintree_secret_livekey', ""),
        '#description' => t('Braintree live secret API Key'),
        '#required' => TRUE
    );
    
    $form['basic_cart_braintree_price'] = array(
        '#type' => 'textfield',
        '#title' => t('Price in USA dollars of download'),
        '#default_value' => variable_get('basic_cart_braintree_price', ""),
        '#description' => t('Price in USA dollars of download'),
        '#required' => TRUE
    );
    
    $form['basic_cart_braintree_gonelive'] = array(
        '#type' => 'checkbox',
        '#title' => t('Live?'),
        '#default_value' => variable_get('basic_cart_braintree_gonelive', ""),
        '#description' => t('Check this box to use your live key.  Uncheck to enter demo mode and use your test key')
    );
    
    $form['basic_cart_braintree_sendemail'] = array(
        '#type' => 'checkbox',
        '#title' => t('Send emails to admin and customer for each successful transaction?'),
        '#default_value' => variable_get('basic_cart_braintree_sendemail', ""),
        '#description' => t('Check this box to send emails.  Uncheck to not send emails.')
    );
  
  $form['basic_cart_braintree_fields_description'] = array(
        '#type' => 'markup',
        '#title' => t('Which fields do you want to collect?'),
        '#markup' => '<hr /><p>You have the choice to collect the following information from your customers below</p>'
    );
    
    $form['basic_cart_braintree_firstname'] = array(
        '#type' => 'checkbox',
        '#title' => t('First Name?'),
        '#default_value' => variable_get('basic_cart_braintree_firstname', ""),
        '#description' => t('Check this box to collect first names.  Uncheck to leave the first name box off of the checkout form.')
    );
    
    $form['basic_cart_braintree_lastname'] = array(
        '#type' => 'checkbox',
        '#title' => t('Last Name?'),
        '#default_value' => variable_get('basic_cart_braintree_lastname', ""),
        '#description' => t('Check this box to collect last names.  Uncheck to leave the last name box off of the checkout form.')
    );
    
    $form['basic_cart_braintree_phone'] = array(
        '#type' => 'checkbox',
        '#title' => t('Phone Number?'),
        '#default_value' => variable_get('basic_cart_braintree_phone', ""),
        '#description' => t('Check this box to collect phone numbers.  Uncheck to leave the phone number box off of the checkout form.')
    );
    
    $form['basic_cart_braintree_address'] = array(
        '#type' => 'checkbox',
        '#title' => t('Address 1?'),
        '#default_value' => variable_get('basic_cart_braintree_address', ""),
        '#description' => t('Check this box to collect address 1.  Uncheck to leave the address 1 box off of the checkout form.')
    );
    
    $form['basic_cart_braintree_address2'] = array(
        '#type' => 'checkbox',
        '#title' => t('Address 2?'),
        '#default_value' => variable_get('basic_cart_braintree_address2', ""),
        '#description' => t('Check this box to collect address 2.  Uncheck to leave the address 2 box off of the checkout form.')
    );
    
    $form['basic_cart_braintree_city'] = array(
        '#type' => 'checkbox',
        '#title' => t('City?'),
        '#default_value' => variable_get('basic_cart_braintree_city', ""),
        '#description' => t('Check this box to collect city.  Uncheck to leave the city box off of the checkout form.')
    );
    
    $form['basic_cart_braintree_county'] = array(
        '#type' => 'checkbox',
        '#title' => t('County?'),
        '#default_value' => variable_get('basic_cart_braintree_county', ""),
        '#description' => t('Check this box to collect county.  Uncheck to leave the county box off of the checkout form.')
    );
    
    $form['basic_cart_braintree_state'] = array(
        '#type' => 'checkbox',
        '#title' => t('State?'),
        '#default_value' => variable_get('basic_cart_braintree_state', ""),
        '#description' => t('Check this box to collect state.  Uncheck to leave the state box off of the checkout form.')
    );
    
    $form['basic_cart_braintree_zip'] = array(
        '#type' => 'checkbox',
        '#title' => t('Zip?'),
        '#default_value' => variable_get('basic_cart_braintree_zip', ""),
        '#description' => t('Check this box to collect zip codes.  Uncheck to leave the zip code box off of the checkout form.')
    );
  
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));

  return $form;
}




















/**
 * Callback for the admin messages.
 */
function basic_cart_braintree_admin_checkout() {
  $form['messages'] = array(
    '#title' => t('Email messages'),
    '#type' => 'fieldset',
    '#description' => t('Here you can customize the mails sent to the site administrator and customer, after an order is placed.'),
  );
  
  $site_mail = variable_get('site_mail');
  $form['messages']['basic_cart_braintree_admin_emails'] = array(
    '#title' => t('Administrator emails'),
    '#type' => 'textarea',
    '#description' => t('After each placed order, an email with the order details will be sent to all the addresses from the list above. 
                          Please add one email address per line.'),
    '#default_value' => variable_get('basic_cart_braintree_admin_emails', $site_mail),
  );

  $form['messages']['basic_cart_braintree_admin_subject'] = array(
    '#title' => t('Subject'),
    '#type' => 'textfield',
    '#description' => t("Subject field for the administrator's email."),
    '#default_value' => variable_get('basic_cart_braintree_admin_subject'),
  );

  $form['messages']['basic_cart_braintree_admin_message'] = array(
    '#title' => t('Admin email'),
    '#type' => 'textarea',
    '#description' => t('This email will be sent to the site administrator just after an order is placed. 
      Availabale tokes: %CUSTOMER_NAME, %CUSTOMER_EMAIL, %CUSTOMER_PHONE, %CUSTOMER_CITY, %CUSTOMER_ZIPCODE, %CUSTOMER_ADDRESS, %CUSTOMER_MESSAGE, %ORDER_DETAILS.'),
    '#default_value' => variable_get('basic_cart_braintree_admin_message'),
  );

  $form['messages']['basic_cart_braintree_send_user_message'] = array(
    '#title' => t('Send an email to the customer after an order is placed'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('basic_cart_braintree_send_user_message'),
  );

  $form['messages']['basic_cart_braintree_user_subject'] = array(
    '#title' => t('Subject'),
    '#type' => 'textfield',
    '#description' => t("Subject field for the user's email."),
    '#default_value' => variable_get('basic_cart_braintree_user_subject'),
  );

  $form['messages']['basic_cart_braintree_user_message'] = array(
    '#title' => t('User email'),
    '#type' => 'textarea',
    '#description' => t('This email will be sent to the user just after an order is placed. Availabale tokes: %CUSTOMER_NAME, %CUSTOMER_EMAIL, %CUSTOMER_PHONE, %CUSTOMER_ADDRESS, %CUSTOMER_MESSAGE, %ORDER_DETAILS'),
    '#default_value' => variable_get('basic_cart_braintree_user_message'),
  );

  $form['thank_you'] = array(
    '#title' => t('Thank you page'),
    '#type' => 'fieldset',
    '#description' => t('Thank you page customization.'),
  );

  $form['thank_you']['basic_cart_braintree_thank_you_title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#description' => t('Thank you page title.'),
    '#default_value' => variable_get('basic_cart_braintree_thank_you_title'),
  );

  $form['thank_you']['basic_cart_braintree_thank_you_message'] = array(
    '#title' => t('Text'),
    '#type' => 'textarea',
    '#description' => t('Thank you page text.'),
    '#default_value' => variable_get('basic_cart_braintree_thank_you_message'),
  );

  return system_settings_form($form);
}

/**
 * Callback validation function for the settings page.
 */
function basic_cart_braintree_admin_content_type_validate($form, &$form_state) {
  $vat_is_enabled = (int) $form_state['values']['basic_cart_braintree_vat_state'];
  if (!empty ($vat_is_enabled) && $vat_is_enabled) {
    $vat_value = (float) $form_state['values']['basic_cart_braintree_vat_value'];
    if ($vat_value <= 0) {
      form_set_error('basic_cart_braintree_vat_value', t('Please enter a valid figure for the VAT.'));
    }
  }
}

/**
 * Callback submit function for the settings page.
 */
function basic_cart_braintree_admin_content_type_submit($form, &$form_state) {
  // Remove internal Form API values.
  unset($form_state['values']['form_id'], $form_state['values']['form_token'], $form_state['values']['form_build_id'], $form_state['values']['op']);

  $product_types = array();
  // Setting up the price field for the selected content types.
  $content_types = $form_state['values']['basic_cart_braintree_content_types'];
  // unset to prevent 'double' save
  unset($form_state['values']['basic_cart_braintree_content_types']);
  
  if (!empty($content_types) && is_array($content_types)){
    // Check to see if the price field already exists.
    $field = field_info_field('price');
    // If the price field does not exist then create it.
    if (empty($field)) {
      $field = array(
        'field_name' => 'price',
        'type' => 'number_decimal',
        'entity_types' => array('node'),
      );
      field_create_field($field);
    }
    foreach ($content_types as $type => $checked) {
      // If a node type is checked, then create the price field.
      if ($checked) {
        // save content_type as a product
        $product_types[$type] = $type;
        // Foreach checked content type, we must assign the price field to the content type.
        $instance = field_info_instance('node', 'price', $type);
        if (empty($instance)) {
          $instance = array(
            'field_name' => 'price',
            'label' => t('Price'),
            'description' => t('Please enter the price for this item.'),
            'entity_type' => 'node',
            'bundle' => $type,
          );
          // It doesn't exist. Create it.
          field_create_instance($instance);
        }
      }
      // If not, then delete the instance.
      else {
        $instance = field_info_instance('node', 'price', $type);
        if (!empty($instance)) {
          field_delete_instance($instance);
        }
      }
    }
  }

  variable_set('basic_cart_braintree_content_types', $product_types);
  
  // Set VAT to nothing if the checkbox is unchecked.
  if (empty($form_state['values']['basic_cart_braintree_vat_state'])) {
    $form_state['values']['basic_cart_braintree_vat_value'] = '';
    // Check to see if the VAT instance exists and if so, delete it.
    if (module_exists('basic_cart_braintree_order')) {
      $instance = field_info_instance('node', 'vat', 'order');
      if (!empty($instance)) {
        field_delete_instance($instance);
      }
    }
  }
  else {
    // If the enable VAT checkbox is checked and if basic_cart_braintree_order is enabled,
    // then create the vat field for the Order content type.
    if (module_exists('basic_cart_braintree_order')) {
      // Check to see if the vat field already exists.
      $field = field_info_field('vat');
      // If the vat field does not exist then create it.
      if (empty($field)) {
        $field = array(
          'field_name' => 'vat',
          'type' => 'number_decimal',
          'entity_types' => array('node'),
        );
        field_create_field($field);

        // Assign the vat field to the Order content type.
        $instance = field_info_instance('node', 'vat', 'order');
        if (empty($instance)) {
          $instance = array(
            'field_name' => 'vat',
            'label' => t('VAT'),
            'description' => t('The VAT tax.'),
            'entity_type' => 'node',
            'bundle' => 'order',
          );
          // It doesn't exist. Create it.
          field_create_instance($instance);
        }
      }
    }
  }
  
  // Save other variables.
  foreach ($form_state['values'] as $key => $value) {
    if (is_array($value) && isset($form_state['values']['array_filter'])) {
      $value = array_keys(array_filter($value));
    }
    variable_set($key, $value);
  }
  
  drupal_set_message(t('The configuration options have been saved.'));
}