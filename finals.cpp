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
    string getPassword() const { return password; }
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
    ofstream fout("users.txt");
    for (const auto& user : users) {
        fout << user.getUsername() << " " << user.getPassword() << "\n";
    }
    fout.close();
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

void registerUser(vector<User>& users, vector<Car>& cars) {
    string username, password;

    cout << "Enter new username: ";
    cin >> username;

    for (const auto& user : users) {
        if (user.getUsername() == username) {
            cout << "Username already exists. Try again.\n";
            return;
        }
    }

    cout << "Enter new password: ";
    cin >> password;

    users.emplace_back(username, password);
    users.back().setCars(&cars);

    saveUsersToFile(users);

    cout << "Registration successful! You can now log in.\n";
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    return to_string(1900 + ltm->tm_year) + "-" + to_string(1 + ltm->tm_mon) + "-" + to_string(ltm->tm_mday);
}

string calculateEndDate(const string& startDate, int days) {
    size_t dash1 = startDate.find('-');
    size_t dash2 = startDate.find('-', dash1 + 1);
    
    int year = stoi(startDate.substr(0, dash1));
    int month = stoi(startDate.substr(dash1 + 1, dash2 - dash1 - 1));
    int day = stoi(startDate.substr(dash2 + 1));
    
    day += days;
    
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
             << setw(20)
<< car.getModel()
<< setw(15) << (car.getAvailability() ? "Yes" : "No") << endl;
}
}

void Admin::updateCar(const string& id, const string& newModel) {
for (auto& car : cars) {
if (car.getId() == id) {
car.setModel(newModel);
saveCarsToFile(cars);
cout << "Car updated successfully.\n";
return;
}
}
cout << "Car ID not found.\n";
}

void Admin::deleteCar(const string& id) {
auto it = remove_if(cars.begin(), cars.end(), [&](const Car& c) { return c.getId() == id; });
if (it != cars.end()) {
cars.erase(it, cars.end());
saveCarsToFile(cars);
cout << "Car deleted successfully.\n";
} else {
cout << "Car ID not found.\n";
}
}

void Admin::filterCarsByModel(const string& keyword) const {
cout << "\nFiltered Cars containing "" << keyword << "":\n";
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

void Admin::viewAllReservations() const {
cout << "\nAll Reservations:\n";
cout << left << setw(15) << "Car ID" << setw(15) << "Username" << setw(15) << "Start Date"
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
saveReservationsToFile(reservations);
cout << "Reservation status updated.\n";
return;
}
}
cout << "Reservation not found.\n";
}

void userMenu(User& user) {
int choice;
do {
cout << "\nUser Menu:\n1. View Available Cars\n2. Rent Car\n3. View My Reservations\n4. Logout\nChoose: ";
cin >> choice;
if (choice == 1) {
user.viewAvailableCars();
} else if (choice == 2) {
string carId;
int days;
cout << "Enter Car ID to rent: ";
cin >> carId;
cout << "Enter number of days: ";
cin >> days;
if (days <= 0) {
cout << "Invalid number of days.\n";
continue;
}
try {
// For demo, we use StandardPricing always, can be extended for more strategies.
StandardPricing strategy;
user.rentCar(carId, &strategy, days);
} catch (const exception& e) {
cout << e.what() << endl;
}
} else if (choice == 3) {
user.viewMyReservations();
}
} while (choice != 4);
}

void adminMenu(Admin& admin) {
int choice;
do {
cout << "\nAdmin Menu:\n1. View Cars\n2. Add Car\n3. Update Car\n4. Delete Car\n5. Filter Cars\n6. View Reservations\n7. Update Reservation Status\n8. Logout\nChoose: ";
cin >> choice;
if (choice == 1) {
admin.viewCars();
} else if (choice == 2) {
string id, model;
cout << "Enter new Car ID: ";
cin >> id;
cout << "Enter new Car Model: ";
cin >> model;
admin.addCar(id, model);
} else if (choice == 3) {
string id, model;
cout << "Enter Car ID to update: ";
cin >> id;
cout << "Enter new Model: ";
cin >> model;
admin.updateCar(id, model);
} else if (choice == 4) {
string id;
cout << "Enter Car ID to delete: ";
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
string carId, username, status;
cout << "Enter Car ID of reservation: ";
cin >> carId;
cout << "Enter Username of reservation: ";
cin >> username;
cout << "Enter new status (Pending/Confirmed/Cancelled): ";
cin >> status;
admin.updateReservationStatus(carId, username, status);
}
} while (choice != 8);
}

int main() {
    vector<Car> cars;
    vector<User> users;
    vector<Reservation> reservations;

    loadCarsFromFile(cars);
    loadUsersFromFile(users, cars);
    loadReservationsFromFile(reservations);

    Admin admin;
    admin.getCars() = cars;
    admin.getReservations() = reservations;

    for (auto& user : users) {
        user.setReservations(&reservations);
    }

    int mainOption;
    do {
        cout << "\nWelcome to Car Rental System\n";
        cout << "1. Login as User\n";
        cout << "2. Login as Admin\n";
        cout << "3. Exit\n";
        cout << "Choose: ";
        cin >> mainOption;

        if (mainOption == 1) {
            int userOption;
            do {
                cout << "\nUser Menu:\n";
                cout << "1. Register\n";
                cout << "2. Login\n";
                cout << "3. Back\n";
                cout << "Choose: ";
                cin >> userOption;

                if (userOption == 1) {
                    registerUser(users, cars);
                } else if (userOption == 2) {
                    string username, password;
                    cout << "Username: ";
                    cin >> username;
                    cout << "Password: ";
                    cin >> password;
                    bool found = false;
                    for (auto& user : users) {
                        if (user.login(username, password)) {
                            found = true;
                            cout << "Login successful.\n";
                            user.setReservations(&reservations);
                            userMenu(user);
                            break;
                        }
                    }
                    if (!found) {
                        cout << "Invalid credentials.\n";
                    }
                }
            } while (userOption != 3);

        } else if (mainOption == 2) {
            string adminPass;
            cout << "Enter admin password: ";
            cin >> adminPass;
            if (adminPass == "group2finalproject") {
                cout << "Admin login successful.\n";
                adminMenu(admin);
            } else {
                cout << "Invalid admin password.\n";
            }
        }
    } while (mainOption != 3);

    return 0;
};
