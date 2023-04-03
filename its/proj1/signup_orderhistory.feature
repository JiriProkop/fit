Feature: signup in and purchase history

    Scenario: 6. Register - success
        Given Register Account page is displayed
        When user enters valid data
        And click Continue
        Then Your Account Has Been Created! page is shown

    Scenario: 7. Register - failure
        Given Register Account page is displayed
        When user enters invalid data
        And click Continue
        Then notification appears with an explanation

    Scenario: 8. displaying order details
        Given Order History is displayed with at least 1 order
        When user click View icon
        Then Order details page is displayed

    Scenario: 9. return order
        Given Order details page is displayed
        When user clicks Return icon
        And valid information is entered
        Then Product Returns page is displayed

    Scenario: 10. Reorder
        Given Order details page is displayed
        When user clicks Reorder icon
        Then items from order are added to shopping cart
