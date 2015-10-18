<?php
/**
 * @file
 * Basic Cart Plus shopping cart block
 */
?>

<?php if (empty($cart)): ?>
  <p><?php print t('Your cart is empty.'); ?></p>
<?php else: ?>
  <div class="basic-cart-grid basic-cart-block">
<?php if(is_array($cart) && count($cart) >= 1): ?>
      <?php
foreach ($cart as $cid => $value) {
if (is_numeric($cid))
{
$node = node_load($cart[$value]);
$member_info = basic_cart_plus_node_membership_info($node);
print '<div class="basic-cart-cart-contents row">';

if (($member_info["is_member_node"] > 0 && $member_info["is_member_user"] == 0) || ($member_info["is_bronze_node"] > 0 && $member_info["is_bronze_user"] == 0) || ($member_info["is_silver_node"] > 0 && $member_info["is_silver_user"] == 0) || ($member_info["is_gold_node"] > 0 && $member_info["is_gold_user"] == 0) )
{
$sitemembership = "yes";
print '<div class="basic-cart-cart-node-title cell">' . l("Site Membership", 'node/' . $node->nid) . '</div>';print '<div class="basic-cart-cart-quantity cell">' . $_SESSION['basic_cart_plus']['cart'][$cid. 'quantity'] . '</div>';
print '<div class="basic-cart-cart-x cell">x</div>';
print '<div class="basic-cart-cart-unit-price cell">';
}
}

if (!isset($sitemembership))
{
print '<div class="basic-cart-cart-node-title cell">' . l($node->title, 'node/' . $node->nid) . '</div>';
print '<div class="basic-cart-cart-quantity cell">' . $_SESSION['basic_cart_plus']['cart'][$cid. 'quantity'] . '</div>';
print '<div class="basic-cart-cart-x cell">x</div>';
print '<div class="basic-cart-cart-unit-price cell">';
print '<strong>' . basic_cart_plus_price_format($node->price["und"][0]["value"]) . '</strong>';
}

print '</div>';
print '</div>';
}
}
?>
      <div class="basic-cart-cart-total-price-contents row">
        <div class="basic-cart-total-price cell">
          <?php print t('Total price'); ?>:<strong> <?php print $price; ?></strong>
        </div>
      </div>
      <?php if (!empty ($vat)): ?>
        <div class="basic-cart-block-total-vat-contents row">
          <div class="basic-cart-total-vat cell"><?php print t('Total VAT'); ?>: <strong><?php print $vat; ?></strong></div>
        </div>
      <?php endif; ?>
      <div class="basic-cart-cart-checkout-button basic-cart-cart-checkout-button-block row">
        <?php print l(t('View cart'), 'cart', array('attributes' => array('class' => array('button')))); ?>
      </div>
    <?php endif; ?>
  </div>
<?php endif; ?>
