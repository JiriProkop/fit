Feature: searching/sorting/buying

  Scenario: 1. Search
    Given homepage is displayed
    When user enters "samsung" into the search bar
    Then Samsung products are shown

  Scenario: 2. Sort products
    Given all laptops and notebooks are shown
    When user clicks Sort by Price(Low > High)
    Then products are sorted from cheapest to the most expensive

  Scenario: 3. Add to a shopping cart
    Given all cameras are displayed
    When user clicks the put in cart icon
    Then a notification indicating success is displayed.

  Scenario: 4. Checkout - out of stock
    Given Shopping cart page is shown with product which is not in stock
    When user clicks Checkout
    Then notification is displayed with text explaining why it's not possible

  Scenario: 5. Checkout
    Given checkout page is displayed with at least 1 item in shopping cart
    When user fills in all needed information
    And clicks Confirm Order
    Then Your order has been placed! page is displayed
