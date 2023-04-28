Feature: signup in and purchase history

    Scenario: 5. Register - success
        Given Register Account page is displayed
        When user enters valid data
        And click Continue
        Then Your Account Has Been Created! page is shown

    Scenario: 6. Register - failure
        Given Register Account page is displayed
        When user enters invalid data
        And click Continue
        Then Your Account Has Been Created! page is NOT shown

    Scenario: 7. displaying order details
        Given user at product page
        When user putts it in cart
        And checks out with register
        And goes to Order History
        Then said product is there

