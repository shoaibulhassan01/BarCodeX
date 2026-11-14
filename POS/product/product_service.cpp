#include "product_service.h"

namespace Pos
{
// Owner: Umar Khan. Loop through products and return the matching barcode.
bool findProductByBarcode(QVector<Product> products, QString barcode,
    Product &foundProduct)
{
    for (Product product : products)
    {
        if (product.barcode == barcode)
        {
            foundProduct = product;
            return true;
        }
    }
    return false;
}

// Owner: Umar Khan. The requested amount can equal the available quantity.
bool checkStock(Product product, int requestedQuantity)
{
    return requestedQuantity > 0 && requestedQuantity <= product.quantity;
}
}
