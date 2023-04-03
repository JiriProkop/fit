Feature: orders management

    Scenario: 15. New order
        Given loged in as administrator
        And Orders page is shown
        When new order is added
        And admin click refreshes page
        Then the new order is shown

    Scenario: 16. Cancel order
        Given loged in as administrator
        And Order details page is shown
        When admin changes Order status to canceled
        And clicks Add history
        Then notification indicating success is shown
        And order status is changed to canceled

    Scenario: 17. Filter by status
        Given loged in as administrator
        And Orders page is shown
        When admin chooses Filter by order status
        Then only orders with given status are shown

    Scenario: 18. Delete order
        Given loged in as administrator
        And Orders page is shown
        When admin chooses order/s
        And clicks Delete icon
        And confirms
        Then notification indicating success is shown
        And order is deleted

    Scenario: 19. Remove product from order
        Given loged in as administrator
        And Order details page is shown
        When admin clicks remove product icon
        Then notification indicating success is shown
        And product is removed from the order
