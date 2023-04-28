Feature: orders management

    Scenario: 10. New order
        Given loged in as administrator
        And Order details page is shown
        And admin changes Order status to canceled
        And clicks Add history
        Then order status is changed to canceled


    Scenario: 11. Delete order
        Given loged in as administrator
        And Orders page is shown
        When admin chooses order/s
        And clicks Delete icon
        And confirms
        Then order is deleted


