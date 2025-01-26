#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

#define FILE_NAME "RestaurantBill.bin"
#define INVOICE_COUNTER_FILE "InvoiceCounter.bin"

class Item {
public:
    std::string name;
    float price;
    int quantity;

    Item(std::string itemName, float itemPrice, int itemQuantity)
        : name(std::move(itemName)), price(itemPrice), quantity(itemQuantity) {}

    void display() const {
        std::cout << name << "\t\t" << quantity << "\t\t" << calculateTotal() << std::endl;
    }

    float calculateTotal() const {
        return quantity * price;
    }
};

class Customer {
public:
    std::string name;
    std::string phone;

    Customer(std::string customerName, std::string customerPhone)
        : name(std::move(customerName)), phone(std::move(customerPhone)) {}
};

class Order {
private:
    static int invoiceCounter;

public:
    int invoiceNumber;
    Customer customerInfo;
    std::string date;
    std::vector<Item> items;

    Order(const Customer& customer)
        : invoiceNumber(++invoiceCounter), customerInfo(customer), date(__DATE__) {}

    void addItem(const Item& item) {
        items.push_back(item);
    }

    void generateBill() const {
        float total = 0;

        generateBillHeader();

        for (const auto& item : items) {
            item.display();
            total += item.calculateTotal();
        }

        generateBillFooter(total);
    }

    void saveToFile() const {
        std::ofstream outFile(FILE_NAME, std::ios::binary | std::ios::app);
        if (outFile.is_open()) {
            outFile.write(reinterpret_cast<const char*>(this), sizeof(Order));
            std::cout << "Successfully saved" << std::endl;
            outFile.close();
        } else {
            std::cerr << "Error opening file " << FILE_NAME << " for writing" << std::endl;
        }
    }

    static void loadAndUpdateInvoiceCounter() {
        std::ifstream counterFile(INVOICE_COUNTER_FILE, std::ios::binary);
        if (counterFile.is_open()) {
            counterFile.read(reinterpret_cast<char*>(&invoiceCounter), sizeof(int));
            counterFile.close();
        }

        // Update the counter for the next use
        std::ofstream updateCounterFile(INVOICE_COUNTER_FILE, std::ios::binary);
        if (updateCounterFile.is_open()) {
            invoiceCounter++;
            updateCounterFile.write(reinterpret_cast<const char*>(&invoiceCounter), sizeof(int));
            updateCounterFile.close();
        } else {
            std::cerr << "Error opening file " << INVOICE_COUNTER_FILE << " for writing" << std::endl;
        }
    }

private:
    void generateBillHeader() const {
        std::cout << "\n\n";
        std::cout << "\t\tASOOM Restaurant";
        std::cout << "\n\t\t----------------";
        std::cout << "\nInvoice Number: " << invoiceNumber;
        std::cout << "\nDate: " << date;
        std::cout << "\nInvoice To: " << customerInfo.name;
        std::cout << "\nPhone: " << customerInfo.phone;
        std::cout << "\n--------------------------------------\n";
        std::cout << "Items\t\t";
        std::cout << "Qty\t\t";
        std::cout << "Total\t\t";
        std::cout << "\n--------------------------------------\n\n";
    }

    void generateBillFooter(float total) const {
        std::cout << "\n";
        float discount = 0.1 * total;
        float netTotal = total - discount;
        float TAX = 0.09 * netTotal;
        float grandTotal = netTotal + TAX;
        std::cout << "\n--------------------------------------\n";
        std::cout << "Sub Total\t\t\t" << netTotal;
        std::cout << "\nDiscount @10%\t\t\t" << discount;
        std::cout << "\n\t\t\t\t-------";
        std::cout << "\nNet Total\t\t\t" << netTotal;
        std::cout << "\nTAX @9%\t\t\t\t" << TAX;
        std::cout << "\n--------------------------------------\n";
        std::cout << "\nGrand Total\t\t\t" << grandTotal;
        std::cout << "\n--------------------------------------\n";
    }
};

int Order::invoiceCounter = 0;

// Function to flush input buffer
void flushInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Function to generate an item menu
Item generateItemMenu() {
    std::string itemName;
    float itemPrice;
    int itemQuantity;

    std::cout << "Enter item name: ";
    std::getline(std::cin, itemName);

    std::cout << "Enter item price: ";
    std::cin >> itemPrice;

    std::cout << "Enter item quantity: ";
    std::cin >> itemQuantity;

    flushInputBuffer(); // Flush input buffer

    return Item(itemName, itemPrice, itemQuantity);
}

int main() {
    Order::loadAndUpdateInvoiceCounter();
    std::vector<Order> orders;

    char contFlag = 'y';

    while (contFlag == 'y') {
        std::cout << "\t==========ASOOM Restaurant==========";
        std::cout << "\n\nPlease select your preferred operation:\t";
        std::cout << "\n\n1. Generate Invoice";
        std::cout << "\n2. Show all Invoices";
        std::cout << "\n3. Search Invoice";
        std::cout << "\n4. Delete Invoice";
        std::cout << "\n5. Menu"; // New menu option
        std::cout << "\n6. Exit";

        int opt;
        std::cout << "\n\nYour choice: ";
        std::cin >> opt;

        switch (opt) {
            case 1: { // Generate Invoice
                Customer customer("Default Customer", "Default Phone");
                Order currentOrder(customer);

                char addAnotherItem = 'y';
                do {
                    currentOrder.addItem(generateItemMenu());
                    std::cout << "Do you want to add another item? [y/n]: ";
                    std::cin >> addAnotherItem;
                    flushInputBuffer();
                } while (addAnotherItem == 'y');

                currentOrder.generateBill();
                currentOrder.saveToFile();
                orders.push_back(currentOrder);
                break;
            }
            case 2: { // Show all Invoices
                for (const auto& order : orders) {
                    order.generateBill();
                }
                break;
            }
            case 3: // Search Invoice (implement if needed)
                break;
            case 4: // Delete Invoice (implement if needed)
                break;
            case 5: { // Menu (New option)
                int menuChoice;
                std::cout << "\n1. Add an item";
                std::cout << "\n2. Delete an item";
                std::cout << "\n3. Update an item";
                std::cout << "\nYour choice: ";
                std::cin >> menuChoice;

                switch (menuChoice) {
                    case 1: { // Add an item
                        orders.back().addItem(generateItemMenu());
                        std::cout << "Item added successfully!" << std::endl;
                        break;
                    }
                    case 2: // Delete an item (implement if needed)
                        break;
                    case 3: // Update an item (implement if needed)
                        break;
                    default:
                        std::cout << "Invalid choice" << std::endl;
                        break;
                }
                break;
            }
            case 6: // Exit
                std::cout << "\n\t\t Exit Successfully\n\n";
                return 0;
            default:
                std::cout << "\nInvalid option.\n";
                break;
        }

        std::cout << "\nDo you want to perform another operation? [y/n]: ";
        std::cin >> contFlag;
        flushInputBuffer(); // Flush input buffer
    }

    return 0;
}
