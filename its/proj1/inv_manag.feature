Feature: inventory management

    Scenario: 11. Add product start
        Given loged in as administrator
        And Products page is shown
        When admin clicks Add New icon
        Then Add Product page is shown

    Scenario: 12. Add product finish
        Given loged in as administrator
        And Add product page is shown
        When admin fills in all required information
        And clicks Save icon
        Then notification indicating success is shown
        And new product is added

    Scenario: 13. Delete product
        Given loged in as administrator
        And Products page with at least 1 product is shown
        When admin chooses product/s
        And clicks Delete icon
        And confirms
        Then product is deleted
        
    Scenario: 14. Change product stock settings
        Given loged in as administrator
        And Edit Product page is displayed
        When admin clicks data
        And changes Stock quantity
        And clicks Save icon
        Then notification indicating success is shown
        And product stock quantity is changed
