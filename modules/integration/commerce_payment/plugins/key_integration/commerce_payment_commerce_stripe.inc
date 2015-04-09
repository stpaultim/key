<?php

/**
 * @file
 * Plugin definition for the Stripe payment method.
 */

$plugin = array(
  'title' => t('Stripe'),
  'description' => t('Integration with the Stripe payment method.'),
  'type' => 'commerce_payment',
  'settings' => array(
    'method_id' => 'commerce_stripe',
    'key_fields' => array('secret_key'),
  ),
);
