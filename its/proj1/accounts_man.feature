Feature: customer accounts management

    Scenario: 20. Create customer group
        Given loged in as administrator
        And Customer groups page is shown
        When admin clicks Add New icon
        And fills in all the details
        And clicks Save icon
        Then notification indicating success is shown
        And customer group is created

    Scenario: 21. Show customer's orders
        Given loged in as administrator
        And Edit Customer page is shown
        When admin clicks Orders icon
        Then all orders by given customer are displayed.
    
    Scenario: 22. Change customer group of customer 
        Given loged in as administrator
        And Edit Customer page is shown
        When admin changes Customer group
        And clicks Save icon
        Then notification indicating success is shown
        And customer's group is changed

    Scenario: 23. Filter customers 
        Given loged in as administrator
        And Customers page is shown
        When admin chooses filter by customer group
        And clicks filter
        Then only customers in given group are shown
