<?php

/**
 * @file
 * Plugin definition for the PayPal WPP payment method.
 */

$plugin = array(
  'title' => t('PayPal WPP'),
  'description' => t('Integration with the PayPal WPP payment method.'),
  'type' => 'commerce_payment',
  'settings' => array(
    'method_id' => 'paypal_wpp',
    'key_fields' => array('api_password'),
  ),
);
