# Key

Key provides the ability to manage keys, which can be employed by other
modules. It gives site administrators the ability to define how and
where keys are stored, which allows the option of a high level of
security and allows sites to meet regulatory or compliance
requirements.

Examples of the types of keys that could be managed with Key are:

* A password or API key for connecting to an external service, such as
PayPal, MailChimp, Authorize.net, UPS, an SMTP mail server, or Amazon
Web Services
* A key used for encrypting data

## Requirements

Key requires the [Plugin Manager](https://www.backdropcms.org/project/plugin_manager)
module.

## Recommended Modules

The following module extends Key's functionality:

* **[Townsend Security Key
Connection](https://www.drupal.org/project/townsec_key)** Allows keys
to be stored on a designated external key management server.

## Installation

Install Key using a standard method for installing a contributed Backdrop
module.

## Configuration

Key provides an administration page where users with the "administer
keys" permission can add, edit, and delete keys.

### Key type

A key type can be selected for a key in order to indicate the purpose
of the key. The following key types are included with Key:

* **Authentication:** A generic key type to use for a password or API
key that does not belong to any other defined key type. This is the
default.
* **Encryption:** Can be used for encrypting and decrypting data. This
key type has a field for selecting a key size, which is used to
validate the size of the key value.

Key types are Plugin Manager plugins, so new types can be defined easily.

### Key provider

A key provider is the means by which the key value is stored and/or
provided when needed. The following key providers are included with
Key:

* **Configuration:** Stores the key in Backdrop configuration settings.
The key value can be set, edited, and viewed through the administrative
interface, making it useful during site development. However, for
better security on production websites, keys should not be stored in
configuration. Keys using the Configuration provider are not obscured
when editing, making it even more important that this provider not be
used in a production environment.
* **File:** Stores the key in a file, which can be anywhere in the file
system, as long as it's readable by the user that runs the web server.
Storing the key in a file outside of the web root is generally more
secure than storing it in the database.

Both the Configuration and File provider plugins support storing the
key with Base64 encoding.

Key providers are Plugin Manager plugins, so new providers can be defined
easily.

### Key input

When adding or editing a key, if the selected key provider accepts a
key value, a key input is automatically selected, as defined by the key
type, in order to submit a key value. The following key inputs are
included with Key:

* **None:** This input is used by default when the selected key
provider does not accept a key value. The File key provider uses this
input.
* **Text Field:** This input provides a basic text field for submitting
a key value. The Configuration key provider uses this input.
* **Textarea Field:** This input is the same as the text field input,
except it uses a textarea HTML element, so it's useful for longer keys,
such as SSH keys.

The Text Field and Textarea Field input plugins support the submission
of keys that are Base64-encoded.

Key inputs are Plugin Manager plugins, so new inputs can be defined easily.

## Generating a Random Encryption Key

Encryption keys have options for key size. The default key size
is 256 bits, with 8 bits per byte. For example, a string like
`12345678901234567890123456789012` has 32 characters/bytes or 256 bits. There is
also an option for specifying if the key was Base64-encoded. Base64 is used here
to encode arbitrary bytes which are known to be safe to send without getting
corrupted.

One way to generate a random encryption key in a Unix environment is to
enter the following command (changing the path and file name to suit your
needs):

`dd if=/dev/urandom bs=32 count=1 > /path/to/secret.key`

This will create a binary file with a random 256-bit key. For a 128-bit key,
change the 32 to 16 in the command. For simplification, consider the "bs" (byte)
to equal a character.

To use base64 encoding when generating the key, use:

`dd if=/dev/urandom bs=32 count=1 | base64 -i - > path/to/secret.key`

Alternatively, you could also create your own 32 character string and search
online for a site that can base64 encode it. Then save it to text file that is
outside the web root and enter the path to the key in the key settings.

## Using a Key

Creating a key will have no effect unless another module makes use of
it. That integration would typically present itself to the end user in
the form of a field that lists available keys and allows the user to
choose one. This could appear, for instance, on the integrating
module's configuration page.

Modules can add a key field to a form using the key_select API element,
which behaves like a select element, but is populated with available
keys as options.

```
$form['secret_key'] = array(
  '#type' => 'key_select',
  '#title' => t('Secret key'),
);
```

There are a couple of additional properties that can be used:

* `#key_filters` An array of filters to apply to the list of keys.
Currently, filtering is quite basic, though it will be improved. You can
filter on key type and/or key provider. Examples:
  * `#key_filters = ['type' => 'mailchimp']` This would only display
    MailChimp keys.
  * `#key_filters = ['provider' => 'file']` This would only display keys
    that use the File key provider.
  * `#key_filters = ['type' => 'mailchimp', 'provider' => 'file']`
    This would only display MailChimp keys that use the File key provider.
* `#key_description` This is a boolean value that determines if information
  about keys is added to the element's description. It is TRUE by default
  and it prepends the description with the following text (with a link to
  the add key form), which can be disabled by setting #key_description to 
  FALSE:

  > Choose an available key. If the desired key is not listed, create a new
    key.

Modules can retrieve configuration for all keys, configuration for a specific
key or the value of a specific key:

### Get all key configurations

`key_get_keys()`

### Get a specific key configuration

`key_get_key($key_id)`

### Get a specific key value

`key_get_key_value($key_id)`

## License

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

## Maintainers

* [Herb v/d Dool](https://github.com/herbdool)
* [Justin Keiser](https://github.com/keiserjb)

## Credits

Ported to Backdrop by [Herb v/d Dool](https://github.com/herbdool) and
[Justin Keiser](https://github.com/keiserjb).

Drupal version currently maintained by:

* <https://www.drupal.org/u/cellar-door>
* <https://www.drupal.org/u/crashtest_>
* <https://www.drupal.org/u/nerdstein>
* <https://www.drupal.org/u/rlhawk>
