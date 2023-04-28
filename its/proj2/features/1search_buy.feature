Feature: searching/sorting/buying

  Scenario: 1. Search
    Given homepage is displayed
    When user enters "samsung" into the search bar
    Then Samsung products are shown

  Scenario: 2. Sort products
    Given all laptops and notebooks are shown
    When user clicks Sort by Price(Low > High)
    Then products are sorted from cheapest to the most expensive

   Scenario: 3. Buy - ok
    Given product page is displayed
    When user adds it to cart
    And clicks Checkout
    And fills in all needed information
    Then Your order has been placed! page is displayed
    
  Scenario: 4. Buy - out of stock
    Given product page is displayed
    When user adds it to cart
    And sets quantity which is not in stock
    And clicks Checkout
    Then checkout won't be displayed
