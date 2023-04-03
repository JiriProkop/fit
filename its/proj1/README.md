# ITS Projekt 1

- **Autor:** Jiří Prokop (xproko47)
- **Datum:** 2023-03-24

## Matice pokrytí artefaktů

Čísla testů jednoznačně identifikují scénář v souborech `.feature`.

| Page                              | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
|-----------------------------------|---|---|---|---|---|---|---|---|---|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| Page Homepage                     | x |   |   |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Page listing products             | x | x | x |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Page Shopping cart                |   |   |   | x |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Page Checkout                     |   |   |   |   | x |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Page Your order has been placed!  |   |   |   |   | x |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Page Register Account             |   |   |   |   |   | x | x |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Page Your account's been created! |   |   |   |   |   | x |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Page Order History                |   |   |   |   |   |   |   | x |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Page Order details - customer     |   |   |   |   |   |   |   | x | x | x  |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Page Product returns              |   |   |   |   |   |   |   |   | x |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Page Products                     |   |   |   |   |   |   |   |   |   |    | x  |    | x  |    |    |    |    |    |    |    |    |    |    |
| Page Add product                  |   |   |   |   |   |   |   |   |   |    | x  | x  |    |    |    |    |    |    |    |    |    |    |    |
| Page Edit product                 |   |   |   |   |   |   |   |   |   |    |    |    |    | x  |    |    |    |    |    |    |    |    |    |
| Page Orders                       |   |   |   |   |   |   |   |   |   |    |    |    |    |    | x  |    | x  |    |    |    | x  |    |    |
| Page Order details - admin        |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    | x  |    | x  | x  |    |    |    |    |
| Page Customer groups              |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    | x  |    |    |    |
| Page Add customer group           |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    | x  |    |    |    |
| Page Customers                    |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    | x  |
| Page Edit customer                |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    | x  | x  |    |


## Matice pokrytí aktivit

| Activities                    | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
|-------------------------------|---|---|---|---|---|---|---|---|---|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| Search product                | x |   |   |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Sort products                 |   | x |   |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Add to cart                   |   |   | x |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Checkout-out of stock         |   |   | x | x |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Checkout                      |   |   | x |   | x |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Register success              |   |   |   |   |   | x |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Register failure              |   |   |   |   |   |   | x |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Display order details         |   |   |   |   | x |   |   | x |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Return order                  |   |   |   |   | x |   |   |   | x |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Reorder                       |   |   |   |   | x |   |   |   |   | x  |    |    |    |    |    |    |    |    |    |    |    |    |    |
| Add product start             |   |   |   |   |   |   |   |   |   |    | x  |    |    |    |    |    |    |    |    |    |    |    |    |
| Add product finish            |   |   |   |   |   |   |   |   |   |    | x  | x  |    |    |    |    |    |    |    |    |    |    |    |
| Delete product                |   |   |   |   |   |   |   |   |   |    | x  | x  | x  |    |    |    |    |    |    |    |    |    |    |
| Change product stock settings |   |   |   |   |   |   |   |   |   |    | x  | x  |    | x  |    |    |    |    |    |    |    |    |    |
| New order in Orders           |   |   |   |   | x |   |   |   |   |    |    |    |    |    | x  |    |    |    |    |    |    |    |    |
| Cancel status in Orders       |   |   |   |   | x |   |   |   |   |    |    |    |    |    |    | x  |    |    |    |    |    |    |    |
| Filter orders by status       |   |   |   |   | x |   |   |   |   |    |    |    |    |    |    |    | x  |    |    |    |    |    |    |
| Delete order                  |   |   |   |   | x |   |   |   |   |    |    |    |    |    |    |    |    | x  |    |    |    |    |    |
| Remove product from order     |   |   |   |   | x |   |   |   |   |    |    |    |    |    |    |    |    |    | x  |    |    |    |    |
| Create customer group         |   |   |   |   |   | x |   |   |   |    |    |    |    |    |    |    |    |    |    | x  |    |    |    |
| Show customer's orders        |   |   |   |   | x |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    | x  |    |    |
| Change c. group of customer   |   |   |   |   |   | x |   |   |   |    |    |    |    |    |    |    |    |    |    | x  |    | x  |    |
| Filter customers by group     |   |   |   |   |   | x |   |   |   |    |    |    |    |    |    |    |    |    |    | x  |    |    | x  |


## Matice Feature-Test

| Feature file                  | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
|-------------------------------|---|---|---|---|---|---|---|---|---|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| search_buy.feature            | x | x | x | x | x |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| signup_orderhistory.feature   |   |   |   |   |   | x | x | x | x | x  |    |    |    |    |    |    |    |    |    |    |    |    |    |
| inv_manag.feature             |   |   |   |   |   |   |   |   |   |    | x  | x  | x  | x  |    |    |    |    |    |    |    |    |    |
| order_manag.feature           |   |   |   |   |   |   |   |   |   |    |    |    |    |    | x  | x  | x  | x  | x  |    |    |    |    |
| accounts_man.feature          |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    | x  | x  | x  | x  |

