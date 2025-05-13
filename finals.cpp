#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <exception>
#include <iomanip>
#include <algorithm>
#include <ctime>

using namespace std;

class Car {
public:
    Car(string id, string model, bool available = true) : id(id), model(model), available(available) {}

    string getId() const { return id; }
    string getModel() const { return model; }
    bool getAvailability() const { return available; }
    void setAvailability(bool availability) { this->available = availability; }
    void setModel(const string& newModel) { model = newModel; }

private:
    string id;
    string model;
    bool available;
};

class PricingStrategy {
public:
    virtual double calculatePrice(int days) = 0;
    virtual ~PricingStrategy() {}
};

class StandardPricing : public PricingStrategy {
public:
    double calculatePrice(int days) override {
        return days * 20.0;
    }
};

class PremiumPricing : public PricingStrategy {
public:
    double calculatePrice(int days) override {
        return days * 30.0;
    }
};

// New Reservation class
class Reservation {
public:
    Reservation(string carId, string username, string startDate, string endDate, double price, string status = "Pending")
        : carId(carId), username(username), startDate(startDate), endDate(endDate), price(price), status(status) {}

    string getCarId() const { return carId; }
    string getUsername() const { return username; }
    string getStartDate() const { return startDate; }
    string getEndDate() const { return endDate; }
    double getPrice() const { return price; }
    string getStatus() const { return status; }
    void setStatus(const string& newStatus) { status = newStatus; }

private:
    string carId;
    string username;
    string startDate;
    string endDate;
    double price;
    string status;
};

class User {
public:
    User(string username, string password) : username(username), password(password) {}

    string getUsername() const { return username; }
    bool login(string user, string pass) {
        return (user == username && pass == password);
    }

    void rentCar(const string& id, PricingStrategy* strategy, int days);
    void viewAvailableCars() const;
    void viewMyReservations() const;

    void logAction(const string& action) {
        ofstream logFile("log.txt", ios::app);
        logFile << action << endl;
    }

    void setCars(vector<Car>* carList) { cars = carList; }
    void setReservations(vector<Reservation>* resList) { reservations = resList; }

private:
    string username;
    string password;
    vector<Car>* cars;
    vector<Reservation>* reservations;
};

void User::viewAvailableCars() const {
    cout << "\nAvailable Cars:\n";
    cout << left << setw(15) << "Car ID" << setw(20) << "Model" << setw(15) << "Available" << endl;
    cout << string(50, '-') << endl;
    for (const auto& car : *cars) {
        if (car.getAvailability()) {
            cout << left << setw(15) << car.getId()
                 << setw(20) << car.getModel()
                 << setw(15) << (car.getAvailability() ? "Yes" : "No") << endl;
        }
    }
}

void User::viewMyReservations() const {
    cout << "\nMy Reservations:\n";
    cout << left << setw(15) << "Car ID" << setw(15) << "Start Date" << setw(15) << "End Date" 
         << setw(15) << "Price" << setw(15) << "Status" << endl;
    cout << string(75, '-') << endl;
    
    for (const auto& res : *reservations) {
        if (res.getUsername() == username) {
            cout << left << setw(15) << res.getCarId()
                 << setw(15) << res.getStartDate()
                 << setw(15) << res.getEndDate()
                 << setw(15) << res.getPrice()
                 << setw(15) << res.getStatus() << endl;
        }
    }
}

class Admin {
public:
    Admin() {}

    void addCar(const string& id, const string& model);
    void viewCars() const;
    void updateCar(const string& id, const string& newModel);
    void deleteCar(const string& id);
    void filterCarsByModel(const string& keyword) const;
    void viewAllReservations() const;
    void updateReservationStatus(const string& carId, const string& username, const string& newStatus);

    vector<Car>& getCars() { return cars; }
    vector<Reservation>& getReservations() { return reservations; }

private:
    vector<Car> cars;
    vector<Reservation> reservations;
};

void saveCarsToFile(const vector<Car>& cars);
void loadCarsFromFile(vector<Car>& cars);
void saveUsersToFile(const vector<User>& users);
void loadUsersFromFile(vector<User>& users, vector<Car>& cars);
void saveReservationsToFile(const vector<Reservation>& reservations);
void loadReservationsFromFile(vector<Reservation>& reservations);

void saveCarsToFile(const vector<Car>& cars) {
    ofstream file("cars.txt");
    for (const auto& car : cars) {
        file << car.getId() << " " << car.getModel() << " " << car.getAvailability() << endl;
    }
    file.close();
}

void loadCarsFromFile(vector<Car>& cars) {
    ifstream file("cars.txt");
    string id, model;
    bool availability;
    while (file >> id >> model >> availability) {
        cars.emplace_back(id, model, availability);
    }
    file.close();
}

void saveUsersToFile(const vector<User>& users) {
    ofstream file("users.txt");
    for (const auto& user : users) {
        file << user.getUsername() << " " << user.getUsername() << endl;
    }
    file.close();
}

void loadUsersFromFile(vector<User>& users, vector<Car>& cars) {
    ifstream file("users.txt");
    string username, password;
    while (file >> username >> password) {
        users.emplace_back(username, password);
        users.back().setCars(&cars);
    }
    file.close();
}

void saveReservationsToFile(const vector<Reservation>& reservations) {
    ofstream file("reservations.txt");
    for (const auto& res : reservations) {
        file << res.getCarId() << " " << res.getUsername() << " " 
             << res.getStartDate() << " " << res.getEndDate() << " "
             << res.getPrice() << " " << res.getStatus() << endl;
    }
    file.close();
}

void loadReservationsFromFile(vector<Reservation>& reservations) {
    ifstream file("reservations.txt");
    string carId, username, startDate, endDate, status;
    double price;
    while (file >> carId >> username >> startDate >> endDate >> price >> status) {
        reservations.emplace_back(carId, username, startDate, endDate, price, status);
    }
    file.close();
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    return to_string(1900 + ltm->tm_year) + "-" + to_string(1 + ltm->tm_mon) + "-" + to_string(ltm->tm_mday);
}

string calculateEndDate(const string& startDate, int days) {
    // Simple date calculation (for demonstration)
    // In a real application, you would use a proper date library
    size_t dash1 = startDate.find('-');
    size_t dash2 = startDate.find('-', dash1 + 1);
    
    int year = stoi(startDate.substr(0, dash1));
    int month = stoi(startDate.substr(dash1 + 1, dash2 - dash1 - 1));
    int day = stoi(startDate.substr(dash2 + 1));
    
    day += days;
    
    // Handle month overflow (simplified)
    if (day > 30) {
        month += day / 30;
        day = day % 30;
    }
    if (month > 12) {
        year += month / 12;
        month = month % 12;
    }
    
    return to_string(year) + "-" + to_string(month) + "-" + to_string(day);
}

void User::rentCar(const string& id, PricingStrategy* strategy, int days) {
    for (auto& car : *cars) {
        if (car.getId() == id && car.getAvailability()) {
            string startDate = getCurrentDate();
            string endDate = calculateEndDate(startDate, days);
            double price = strategy->calculatePrice(days);
            
            // Create new reservation
            reservations->emplace_back(id, username, startDate, endDate, price);
            
            car.setAvailability(false);
            cout << "Car rented successfully. Total price: $" << price << endl;
            logAction("User " + username + " rented car ID " + id + " from " + startDate + " to " + endDate + ". Price: $" + to_string(price));
            
            saveCarsToFile(*cars);
            saveReservationsToFile(*reservations);
            return;
        }
    }
    throw runtime_error("Car not available or ID not found.");
}

void Admin::addCar(const string& id, const string& model) {
    cars.emplace_back(id, model);
    saveCarsToFile(cars);
}

void Admin::viewCars() const {
    cout << "\nAll Cars:\n";
    cout << left << setw(15) << "Car ID" << setw(20) << "Model" << setw(15) << "Available" << endl;
    cout << string(50, '-') << endl;
    for (const auto& car : cars) {
        cout << left << setw(15) << car.getId()
             << setw(20) << car.getModel()
             << setw(15) << (car.getAvailability() ? "Yes" : "No") << endl;
    }
}

void Admin::viewAllReservations() const {
    cout << "\nAll Reservations:\n";
    cout << left << setw(15) << "Car ID" << setw(15) << "User" << setw(15) << "Start Date" 
         << setw(15) << "End Date" << setw(15) << "Price" << setw(15) << "Status" << endl;
    cout << string(90, '-') << endl;
    
    for (const auto& res : reservations) {
        cout << left << setw(15) << res.getCarId()
             << setw(15) << res.getUsername()
             << setw(15) << res.getStartDate()
             << setw(15) << res.getEndDate()
             << setw(15) << res.getPrice()
             << setw(15) << res.getStatus() << endl;
    }
}

void Admin::updateReservationStatus(const string& carId, const string& username, const string& newStatus) {
    for (auto& res : reservations) {
        if (res.getCarId() == carId && res.getUsername() == username) {
            res.setStatus(newStatus);
            
            // If reservation is cancelled, make the car available again
            if (newStatus == "Cancelled") {
                for (auto& car : cars) {
                    if (car.getId() == carId) {
                        car.setAvailability(true);
                        break;
                    }
                }
            }
            
            saveReservationsToFile(reservations);
            saveCarsToFile(cars);
            return;
        }
    }
    throw runtime_error("Reservation not found.");
}

void Admin::updateCar(const string& id, const string& newModel) {
    for (auto& car : cars) {
        if (car.getId() == id) {
            car.setModel(newModel);
            saveCarsToFile(cars);
            return;
        }
    }
    throw runtime_error("Car ID not found.");
}

void Admin::deleteCar(const string& id) {
    for (auto it = cars.begin(); it != cars.end(); ++it) {
        if (it->getId() == id) {
            cars.erase(it);
            saveCarsToFile(cars);
            return;
        }
    }
    throw runtime_error("Car ID not found.");
}

void Admin::filterCarsByModel(const string& keyword) const {
    cout << "\nFiltered Cars (Model contains '" << keyword << "'):\n";
    cout << left << setw(15) << "Car ID" << setw(20) << "Model" << setw(15) << "Available" << endl;
    cout << string(50, '-') << endl;
    for (const auto& car : cars) {
        if (car.getModel().find(keyword) != string::npos) {
            cout << left << setw(15) << car.getId()
                 << setw(20) << car.getModel()
                 << setw(15) << (car.getAvailability() ? "Yes" : "No") << endl;
        }
    }
}

int main() {
    Admin admin;
    vector<User> users;

    loadCarsFromFile(admin.getCars());
    loadUsersFromFile(users, admin.getCars());
    loadReservationsFromFile(admin.getReservations());
    
    // Set reservations pointers for users
    for (auto& user : users) {
        user.setReservations(&admin.getReservations());
    }

    bool running = true;
    while (running) {
        cout << "\nSelect Role:\n1. Admin\n2. User\n3. Exit\nChoice: ";
        int role;
        cin >> role;

        if (role == 1) {
            string password;
            cout << "Enter Admin Password: ";
            cin >> password;

            if (password == "group2finalproject") {
                bool adminLoggedIn = true;
                while (adminLoggedIn) {
                    cout << "\nAdmin Menu:\n1. Add Car\n2. View Cars\n3. Update Car\n4. Delete Car\n5. Filter Cars\n6. View Reservations\n7. Update Reservation Status\n8. Logout\nChoice: ";
                    int choice;
                    cin >> choice;

                    try {
                        if (choice == 1) {
                            string id, model;
                            cout << "Enter Car ID: ";
                            cin >> id;
                            cout << "Enter Car Model: ";
                            cin >> model;
                            admin.addCar(id, model);
                        } else if (choice == 2) {
                            admin.viewCars();
                        } else if (choice == 3) {
                            admin.viewCars();
                            string id, newModel;
                            cout << "Enter Car ID to Update: ";
                            cin >> id;
                            cout << "Enter New Car Model: ";
                            cin >> newModel;
                            admin.updateCar(id, newModel);
                        } else if (choice == 4) {
                            admin.viewCars();
                            string id;
                            cout << "Enter Car ID to Delete: ";
                            cin >> id;
                            admin.deleteCar(id);
                        } else if (choice == 5) {
                            string keyword;
                            cout << "Enter keyword to filter by model: ";
                            cin >> keyword;
                            admin.filterCarsByModel(keyword);
                        } else if (choice == 6) {
                            admin.viewAllReservations();
                        } else if (choice == 7) {
                            admin.viewAllReservations();
                            string carId, username, newStatus;
                            cout << "Enter Car ID: ";
                            cin >> carId;
                            cout << "Enter Username: ";
                            cin >> username;
                            cout << "Enter New Status (Approved/Cancelled/Completed): ";
                            cin >> newStatus;
                            admin.updateReservationStatus(carId, username, newStatus);
                        } else if (choice == 8) {
                            adminLoggedIn = false;
                        } else {
                            cout << "Invalid choice.\n";
                        }
                    } catch (const exception& e) {
                        cout << "Error: " << e.what() << endl;
                    }
                }
            } else {
                cout << "Invalid password.\n";
            }
        } else if (role == 2) {
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            bool loggedIn = false;
            for (User& user : users) {
                if (user.login(username, password)) {
                    loggedIn = true;
                    bool userMenu = true;
                    while (userMenu) {
                        cout << "\nUser Menu:\n1. View Available Cars\n2. Rent a Car\n3. View My Reservations\n4. Logout\nChoice: ";
                        int choice;
                        cin >> choice;

                        try {
                            if (choice == 1) {
                                user.viewAvailableCars();
                            } else if (choice == 2) {
                                user.viewAvailableCars();
                                string carId;
                                int days, pricingChoice;
                                cout << "Enter Car ID to Rent: ";
                                cin >> carId;
                                cout << "Enter number of days: ";
                                cin >> days;
                                cout << "Choose pricing strategy (1 = Standard, 2 = Premium): ";
                                cin >> pricingChoice;
                                unique_ptr<PricingStrategy> strategy;
                                if (pricingChoice == 1) {
                                    strategy = make_unique<StandardPricing>();
                                } else if (pricingChoice == 2) {
                                    strategy = make_unique<PremiumPricing>();
                                } else {
                                    throw runtime_error("Invalid pricing strategy.");
                                }
                                user.rentCar(carId, strategy.get(), days);
                            } else if (choice == 3) {
                                user.viewMyReservations();
                            } else if (choice == 4) {
                                userMenu = false;
                            } else {
                                cout << "Invalid choice.\n";
                            }
                        } catch (const exception& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                    }
                    break;
                }
            }
            if (!loggedIn) {
                cout << "Invalid username or password.\n";
            }
        } else if (role == 3) {
            running = false;
        } else {
            cout << "Invalid role selection.\n";
        }
    }
    return 0;
}