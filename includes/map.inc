<?php

/**
 * @file
 * CivicTheme Accordion paragraph component.
 */

/**
 * Implements template_preprocess_paragraph().
 */
function civictheme_preprocess_paragraph__civictheme_map(&$variables) {
  _civictheme_preprocess_paragraph__paragraph_field__theme($variables);
  _civictheme_preprocess_paragraph__paragraph_field__vertical_spacing($variables);
  _civictheme_preprocess_paragraph__paragraph_field__background($variables);

  /** @var \Drupal\link\Plugin\Field\FieldType\LinkItem|null $link */
  $link = civictheme_get_field_value($variables['paragraph'], 'field_c_p_embed_url', TRUE);
  if ($link) {
    $variables['url'] = $link->getUrl()->toString();
  }

  $variables['address'] = civictheme_get_field_value($variables['paragraph'], 'field_c_p_address');

  /** @var \Drupal\link\Plugin\Field\FieldType\LinkItem|null $link */
  $view_link = civictheme_get_field_value($variables['paragraph'], 'field_c_p_view_link', TRUE);
  if ($view_link) {
    $variables['view_url'] = $view_link->getUrl()->toString();
  }
}
