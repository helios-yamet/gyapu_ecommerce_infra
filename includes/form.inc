<?php

/**
 * @file
 * Form alterations.
 */

use Drupal\civictheme\CivicthemeConstants;
use Drupal\Component\Utility\Html;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;
use Drupal\paragraphs\Entity\Paragraph;

/**
 * Implements hook_form_alter().
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function civictheme_form_alter(&$form, FormStateInterface $form_state, $form_id) {
  _civictheme_form_alter__webform_submission($form, $form_state, $form_id);
  _civictheme_form_alter__non_form_elements($form);
}

/**
 * Implements template_preprocess_select().
 */
function civictheme_preprocess_select(&$variables) {
  _civictheme_preprocess_form__theme($variables);
}

/**
 * Implements template_preprocess_input().
 */
function civictheme_preprocess_input(&$variables) {
  _civictheme_preprocess_form__theme($variables);

  $variables['state'] = 'none';

  if (isset($variables['attributes']['class'])) {
    if (in_array('error', $variables['attributes']['class'])) {
      $variables['state'] = 'error';
    }
  }

  if (isset($variables['attributes']['#civictheme_theme'])) {
    unset($variables['attributes']['#civictheme_theme']);
  }
}

/**
 * Implements template_preprocess_form_element().
 */
function civictheme_preprocess_form_element(&$variables) {
  _civictheme_preprocess_form__theme($variables);
  _civictheme_preprocess_form_element__classes($variables);
  _civictheme_preprocess_form_element__description($variables);
}

/**
 * Implements template_preprocess_form_element_label().
 */
function civictheme_preprocess_form_element_label(&$variables) {
  _civictheme_preprocess_form__theme($variables);
  $variables['content'] = $variables['title'] ?? '';

  // Adding in core template classes.
  if (!empty($variables['required'])) {
    $variables['attributes']['class'][] = 'js-form-required';
  }
}

/**
 * Form alter for webform submission form.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function _civictheme_form_alter__webform_submission(&$form, FormStateInterface $form_state, $form_id) {
  if (!empty($form_id) && strpos($form_id, 'webform_submission_') === 0) {
    /** @var \Drupal\webform\WebformSubmissionForm $form_object */
    $form_object = $form_state->getFormObject();

    /** @var \Drupal\webform\WebformSubmissionInterface $webform_submission */
    $webform_submission = $form_object->getEntity();
    if ($webform_submission) {
      $source = $webform_submission->getSourceEntity();

      $paragraph_id = !empty($source) ? $source->id() : '';
      if (!empty($paragraph_id) && !empty($form['elements'])) {
        $theme = civictheme_get_field_theme_value(Paragraph::load($paragraph_id));
        _civictheme_form__propagate_component_theme($form['elements'], $theme);
      }
    }
  }
}

/**
 * Alter elements in forms that are not categorised as form elements.
 *
 * Form elements should be preprocessed in (and associated preprocess functions)
 *
 * However, there area some types of Element in form are not form elements eg
 * link and html_tag. This function provides the ability to alter these elements
 * if required.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function _civictheme_form_alter__non_form_elements(&$elements) {
  $element_keys = Element::children($elements);
  foreach ($element_keys as $element_key) {
    if (empty($elements[$element_key]['#type'])) {
      continue;
    }
    $nested_elements = Element::children($elements[$element_key]);
    if (!empty($nested_elements)) {
      _civictheme_form_alter__non_form_elements($elements[$element_key]);
    }
    // Add elements to modify to this switch.
    switch ($elements[$element_key]['#type']) {
      case 'link':
        _civictheme_form_alter__non_form_elements__link($elements[$element_key]);
        break;

      default:
        continue 2;
    }
  }
}

/**
 * Alter link elements within forms.
 */
function _civictheme_form_alter__non_form_elements__link(&$element) {
  // Convert "button" links into CivicTheme button links.
  if (!empty($element['#attributes']['class']) && in_array('button', $element['#attributes']['class'])) {
    $element_theme = _civictheme_form__get_component_theme($element);
    $element['#attributes']['class'] = array_merge($element['#attributes']['class'], [
      'ct-button',
      'ct-button--regular',
      'ct-button--secondary',
      'ct-theme-' . $element_theme,
    ]);
  }
}

/**
 * Sets form element theme based on attributes passed from parent components.
 */
function _civictheme_preprocess_form__theme(&$variables) {
  // Setting theme for form element.
  if (
    isset($variables['element']['#attributes']['#civictheme_theme'])
    && in_array($variables['element']['#attributes']['#civictheme_theme'], civictheme_theme_options(TRUE))
  ) {
    $variables['theme'] = $variables['element']['#attributes']['#civictheme_theme'];
    unset($variables['element']['#attributes']['#civictheme_theme']);

    if (isset($variables['element']['label']['#attributes'])) {
      $variables['element']['label']['#attributes']['#civictheme_theme'] = $variables['theme'];
    }

    if (isset($variables['label']['#attributes'])) {
      $variables['label']['#attributes']['#civictheme_theme'] = $variables['theme'];
    }
  }
}

/**
 * Preprocess hidden form fields.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function _civictheme_preprocess_form__hidden_fields($variables) {
  if (empty($variables['form'])) {
    return;
  }

  $form = &$variables['form'];

  $field_names = [
    'form_build_id',
    'form_id',
    'form_token',
  ];

  $output = '';
  foreach (Element::children($form) as $field_name) {
    if (in_array($field_name, $field_names)) {
      $els = [$form[$field_name]];
      $output .= \Drupal::service('renderer')->render($els);
    }
  }

  $variables['form_hidden_fields'] = $output;
}

/**
 * Preprocesses form element classes.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function _civictheme_preprocess_form_element__classes(&$variables) {
  // Handle any malformed attribute class properties.
  $variables['attributes']['class'] = $variables['attributes']['class'] ?? [];
  $variables['attributes']['class'] = is_string($variables['attributes']['class']) ? [$variables['attributes']['class']] : $variables['attributes']['class'];

  // Add missing core Drupal form element classes that are added in template
  // file.
  // Note that we are only adding JS classes for 3rd-parties to bind to.
  $variables['attributes']['class'][] = 'js-form-item';

  if (!empty($variables['type'])) {
    $variables['attributes']['class'][] = 'js-form-type-' . Html::getClass($variables['type']);
  }

  if (!empty($variables['name'])) {
    $variables['attributes']['class'][] = 'js-form-item-' . Html::getClass($variables['name']);
  }

  if (!empty($variables['errors'])) {
    $variables['attributes']['class'][] = 'js-form-item--error';
  }
}

/**
 * Preprocesses form element description.
 */
function _civictheme_preprocess_form_element__description(&$variables) {
  $variables['description'] = $variables['element']['#description'] ?? NULL;
  $variables['description_display'] = $variables['element']['#description_display'] ?? NULL;
}

/**
 * Get fields from the form without hidden fields.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function _civictheme_form__get_fields($form) {
  $exclude = [
    'form_build_id',
    'form_id',
    'form_token',
    'actions',
  ];

  $field_names = Element::children($form);

  return array_intersect_key($form, array_flip(array_diff($field_names, $exclude)));
}

/**
 * Get component theme from the element.
 */
function _civictheme_form__get_component_theme($element) {
  $theme = CivicthemeConstants::THEME_LIGHT;
  if (isset($element['#attributes']['#civictheme_theme'])
    && in_array($element['#attributes']['#civictheme_theme'], [
      CivicthemeConstants::THEME_LIGHT,
      CivicthemeConstants::THEME_DARK,
    ])) {
    $theme = $element['#attributes']['#civictheme_theme'];
  }

  return $theme;
}

/**
 * Propagate component theme setting to nested form elements.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function _civictheme_form__propagate_component_theme(&$elements, $theme) {
  $form_elements = Element::children($elements);
  foreach ($form_elements as $form_element) {
    $elements[$form_element]['#attributes']['#civictheme_theme'] = $theme;
    $elements[$form_element]['#attributes']['#component_theme'] = $theme;
    $children = Element::children($elements[$form_element]);
    if (!empty($children)) {
      _civictheme_form__propagate_component_theme($elements[$form_element], $theme);
    }
  }
}

/**
 * Check if form field is set to have multiple values.
 */
function _civictheme_form__field_is_multiple(array $field) {
  return $field['#type'] == 'checkboxes' || ($field['#type'] == 'select' && $field["#multiple"] ?? FALSE);
}
