Feature: inventory management

    Scenario: 8. Add product start
        Given loged in as administrator
        And Products page is shown
        When admin clicks Add New icon
        And admin fills in all required information
        And clicks Save icon
        Then that product is added

    Scenario: 9. Delete product
        Given loged in as administrator
        And Products page with at least 1 product is shown
        When admin chooses product/s
        And clicks Delete icon
        And confirms
        Then product is deleted
        
