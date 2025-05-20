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
#include <map>
#include <cctype>
#include <limits>

using namespace std;

// Helper: Convert string to uppercase for case-insensitive comparison
string toUpper(const string& s) {
    string out = s;
    transform(out.begin(), out.end(), out.begin(), ::toupper);
    return out;
}

// Helper: Get numeric input with validation
int getNumericInput(const string& prompt) {
    string input;
    int value;
    while (true) {
        cout << prompt;
        cin >> input;
        bool isNumeric = !input.empty() && all_of(input.begin(), input.end(), ::isdigit);
        if (isNumeric) {
            value = stoi(input);
            break;
        } else {
            cout << "Invalid input. Please enter numbers only.\n";
        }
    }
    return value;
}

// --- Car Class with Plate Number and Status ---
class Car {
public:
    Car(string id, string model, string plateNumber, string status = "Available")
        : id(id), model(model), plateNumber(plateNumber), status(status) {}

    string getId() const { return id; }
    string getModel() const { return model; }
    string getPlateNumber() const { return plateNumber; }
    string getStatus() const { return status; }
    void setStatus(const string& newStatus) { status = newStatus; }
    void setModel(const string& newModel) { model = newModel; }
    void setPlateNumber(const string& newPlate) { plateNumber = newPlate; }
    bool isAvailable() const { return status == "Available"; }

private:
    string id;
    string model;
    string plateNumber;
    string status; // Available, Rented, Maintenance, etc.
};

class PricingStrategy {
public:
    virtual double calculatePrice(int days) = 0;
    virtual ~PricingStrategy() {}
};

class StandardPricing : public PricingStrategy {
public:
    double calculatePrice(int days) override {
        return days * 500.0;
    }
};

class PremiumPricing : public PricingStrategy {
public:
    double calculatePrice(int days) override {
        return days * 1000.0;
    }
};

// --- Reservation Class with Payment Status ---
class Reservation {
public:
    Reservation(string carId, string username, string startDate, string endDate, double price, string status = "Pending", string paymentStatus = "Pending")
        : carId(carId), username(username), startDate(startDate), endDate(endDate), price(price), status(status), paymentStatus(paymentStatus) {}

    string getCarId() const { return carId; }
    string getUsername() const { return username; }
    string getStartDate() const { return startDate; }
    string getEndDate() const { return endDate; }
    double getPrice() const { return price; }
    string getStatus() const { return status; }
    string getPaymentStatus() const { return paymentStatus; }
    void setStatus(const string& newStatus) { status = newStatus; }
    void setPaymentStatus(const string& newStatus) { paymentStatus = newStatus; }

private:
    string carId;
    string username;
    string startDate;
    string endDate;
    double price;
    string status;
    string paymentStatus;
};

void saveReservationsToFile(const std::vector<Reservation>& reservations);

// --- User Class with Cancel Reservation and Change Password ---
class User {
public:
    User(string username, string password) : username(username), password(password) {}
    vector<Reservation>* getReservations() const { return reservations; }

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    bool login(string user, string pass) {
        return (user == username && pass == password);
    }

    void rentCar(const string& id, PricingStrategy* strategy, int days, vector<Car>& cars);
    void viewAvailableCars() const;
    void viewMyReservations() const;
    void cancelReservation(const string& carId, vector<Car>& cars);
    void changePassword(const string& newPassword);
    void payForReservation();

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

void User::changePassword(const string& newPassword) {
    if (newPassword.empty()) {
        cout << "Password cannot be empty.\n";
        return;
    }
    password = newPassword;
    cout << "Password changed.\n";
}

void User::viewAvailableCars() const {
    cout << "\nAvailable Cars:\n";
    cout << left << setw(15) << "Car ID" << setw(20) << "Model" << setw(15) << "Plate No."
         << setw(15) << "Status" << setw(15) << "Price/Day" << endl;
    cout << string(80, '-') << endl;
    for (const auto& car : *cars) {
        if (car.isAvailable()) {
            cout << left << setw(15) << car.getId()
                 << setw(20) << car.getModel()
                 << setw(15) << car.getPlateNumber()
                 << setw(15) << car.getStatus()
                 << setw(15) << 500.0 << endl; // Or use car.getPricePerDay() if you add that field
        }
    }
}

// ...existing code...
void User::viewMyReservations() const {
    cout << "\nMy Reservations:\n";
    cout << left << setw(15) << "Car ID" << setw(15) << "Start Date" << setw(15) << "End Date"
         << setw(15) << "Price" << setw(15) << "Status" << setw(15) << "Payment" << endl;
    cout << string(90, '-') << endl;

    bool found = false;
    for (auto& res : *reservations) {
        if (res.getUsername() == username) {
            found = true;
            // If status is Cancelled, set payment status to Cancelled as well
            if (toUpper(res.getStatus()) == "CANCELLED" && res.getPaymentStatus() != "Cancelled") {
                res.setPaymentStatus("Cancelled");
                saveReservationsToFile(*reservations);
            }
            cout << left << setw(15) << res.getCarId()
                 << setw(15) << res.getStartDate()
                 << setw(15) << res.getEndDate()
                 << setw(15) << res.getPrice()
                 << setw(15) << res.getStatus()
                 << setw(15) << res.getPaymentStatus() << endl;
        }
    }
    if (!found) {
        cout << "No reservation\n";
    }
}

void User::payForReservation() {
    bool found = false;
    cout << "\nUnpaid Reservations:\n";
    cout << left << setw(15) << "Car ID" << setw(15) << "Start Date" << setw(15) << "End Date"
         << setw(15) << "Price" << setw(15) << "Status" << setw(15) << "Payment" << endl;
    cout << string(90, '-') << endl;
    vector<string> unpaidCarIds;
    for (const auto& res : *reservations) {
        if (res.getUsername() == username && res.getPaymentStatus() == "Pending" && toUpper(res.getStatus()) == "CONFIRMED") {
            found = true;
            cout << left << setw(15) << res.getCarId()
                 << setw(15) << res.getStartDate()
                 << setw(15) << res.getEndDate()
                 << setw(15) << res.getPrice()
                 << setw(15) << res.getStatus()
                 << setw(15) << res.getPaymentStatus() << endl;
            unpaidCarIds.push_back(toUpper(res.getCarId()));
        }
    }
    if (!found) {
        cout << "No unpaid confirmed reservations found.\n";
        return;
    }
    string carId;
    while (true) {
        cout << "Enter Car ID to pay for: ";
        cin >> carId;
        if (find(unpaidCarIds.begin(), unpaidCarIds.end(), toUpper(carId)) != unpaidCarIds.end()) {
            break;
        } else {
            cout << "Car ID not found in your unpaid confirmed reservations. Please try again.\n";
        }
    }
    for (auto& res : *reservations) {
        if (res.getUsername() == username && toUpper(res.getCarId()) == toUpper(carId) && res.getPaymentStatus() == "Pending" && toUpper(res.getStatus()) == "CONFIRMED") {
            int payMethod;
            cout << "Select payment method:\n1. Cash\n2. Card\nChoose: ";
            payMethod = getNumericInput("");
            if (payMethod == 2) {
                string cardNum;
                cout << "Enter 16-digit card number: ";
                cin >> cardNum;
                while (cardNum.length() != 16 || !all_of(cardNum.begin(), cardNum.end(), ::isdigit)) {
                    cout << "Invalid card number. Enter 16-digit card number: ";
                    cin >> cardNum;
                }
                cout << "Card payment accepted.\n";
            } else {
                cout << "Cash payment accepted.\n";
            }
            res.setPaymentStatus("Paid");
            saveReservationsToFile(*reservations);
            cout << "Payment successful for reservation " << carId << ".\n";
            return;
        }
    }
    cout << "Reservation not found or already paid.\n";
}

void User::cancelReservation(const string& carId, vector<Car>& carsVec) {
    string carIdUpper = toUpper(carId);
    for (auto& res : *reservations) {
        if (toUpper(res.getCarId()) == carIdUpper && res.getUsername() == username && res.getStatus() != "Cancelled") {
            res.setStatus("Cancelled");
            // Update car status to Available if reservation is cancelled
            for (auto& car : carsVec) {
                if (toUpper(car.getId()) == carIdUpper) {
                    car.setStatus("Available");
                    break;
                }
            }
            logAction("User " + username + " cancelled reservation for car ID " + carId);
            saveReservationsToFile(*reservations);
            cout << "Reservation cancelled.\n";
            return;
        }
    }
    cout << "No active reservation found for this car.\n";
}

void cancelReservationWithPrompt(User& user, vector<Car>& cars) {
    // Show user's active reservations

    bool found = false;
    cout << "\nYour Active Reservations:\n";
    cout << left << setw(15) << "Car ID" << setw(15) << "Start Date" << setw(15) << "End Date"
         << setw(15) << "Price" << setw(15) << "Status" << setw(15) << "Payment" << endl;
    cout << string(90, '-') << endl;
    for (const auto& res : *(user.getReservations())) {
        if (res.getUsername() == user.getUsername() && res.getStatus() != "Cancelled") {
            found = true;
            cout << left << setw(15) << res.getCarId()
                 << setw(15) << res.getStartDate()
                 << setw(15) << res.getEndDate()
                 << setw(15) << res.getPrice()
                 << setw(15) << res.getStatus()
                 << setw(15) << res.getPaymentStatus() << endl;
        }
    }
    if (!found) {
        cout << "No active reservation to cancel.\n";
        return;
    }

    while (true) {
        string carId;
        cout << "Enter Car ID to cancel reservation (or 0 to back): ";
        cin >> carId;
        if (carId == "0") return;

        // Confirm cancellation
        cout << "Are you sure you want to cancel reservation for Car ID " << carId << "? (y/n): ";
        string confirm;
        cin >> confirm;
        if (toUpper(confirm) == "Y") {
            user.cancelReservation(carId, cars);
            break;
        } else if (toUpper(confirm) == "N") {
            cout << "Cancellation aborted.\n";
            break;
        } else {
            cout << "Invalid input. Please enter y or n.\n";
        }
    }
}

// --- Admin Class with User Management ---
class Admin {
public:
    Admin() : reservations(nullptr) {}

    void addCar(const string& id, const string& model, const string& plateNumber);
    void viewCars() const;
    void updateCar(const string& id, const string& newModel);
    void deleteCar(const string& id);
    void filterCarsByModel(const string& keyword) const;
    void viewAllReservations() const;
    void updateReservationStatus(const string& carId, const string& username, const string& newStatus, vector<Car>& cars, vector<Reservation>& reservations);
    void viewUsers(const vector<User>& users) const;
    void deleteUser(vector<User>& users, const string& username);

    vector<Car>& getCars() { return cars; }
    void setReservations(vector<Reservation>* resList) { reservations = resList; }
    vector<Reservation>& getReservations() { return *reservations; }
    const vector<Reservation>& getReservations() const { return *reservations; }

private:
    vector<Car> cars;
    vector<Reservation>* reservations; // pointer to global reservations
};

// --- File I/O Updated for New Fields ---
void saveCarsToFile(const vector<Car>& cars) {
    ofstream file("cars.txt");
    for (const auto& car : cars) {
        file << car.getId() << " " << car.getModel() << " " << car.getPlateNumber() << " " << car.getStatus() << endl;
    }
    file.close();
}

void loadCarsFromFile(vector<Car>& cars) {
    cars.clear();
    ifstream file("cars.txt");
    string id, model, plateNumber, status;
    while (file >> id >> model >> plateNumber >> status) {
        if (id.empty() || model.empty() || plateNumber.empty() || status.empty())
            continue; // skip malformed lines
        cars.emplace_back(id, model, plateNumber, status);
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
             << res.getPrice() << " " << res.getStatus() << " " << res.getPaymentStatus() << endl;
    }
    file.close();
}

void loadReservationsFromFile(vector<Reservation>& reservations) {
    ifstream file("reservations.txt");
    string carId, username, startDate, endDate, status, paymentStatus;
    double price;
    while (file >> carId >> username >> startDate >> endDate >> price >> status >> paymentStatus) {
        reservations.emplace_back(carId, username, startDate, endDate, price, status, paymentStatus);
    }
    file.close();
}

void registerUser(vector<User>& users, vector<Car>& cars) {
    string username, password;

    cout << "Enter new username: ";
    cin >> username;
    if (username.empty()) {
        cout << "Username cannot be empty.\n";
        return;
    }
    for (const auto& user : users) {
        if (user.getUsername() == username) {
            cout << "Username already exists. Try again.\n";
            return;
        }
    }

    cout << "Enter new password: ";
    cin >> password;
    if (password.empty()) {
        cout << "Password cannot be empty.\n";
        return;
    }

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

// --- Reservation Conflict Check ---
bool isReservationConflict(const vector<Reservation>& reservations, const string& carId, const string& startDate, const string& endDate) {
    for (const auto& res : reservations) {
        if (res.getCarId() == carId && res.getStatus() != "Cancelled") {
            if (!(endDate < res.getStartDate() || startDate > res.getEndDate())) {
                return true;
            }
        }
    }
    return false;
}

// --- User::rentCar with Conflict Check and Car Status ---
void User::rentCar(const string& id, PricingStrategy* strategy, int days, vector<Car>& carsVec) {
    string idUpper = toUpper(id);
    if (days <= 0) {
        cout << "Number of days must be positive.\n";
        return;
    }
    auto carIt = find_if(carsVec.begin(), carsVec.end(), [&](const Car& c) { return toUpper(c.getId()) == idUpper; });
    if (carIt == carsVec.end()) {
        cout << "Car ID not found.\n";
        return;
    }
    if (!carIt->isAvailable()) {
        cout << "Car is not available.\n";
        return;
    }
    string startDate = getCurrentDate();
    string endDate = calculateEndDate(startDate, days);
    if (isReservationConflict(*reservations, idUpper, startDate, endDate)) {
        cout << "Reservation conflict: Car is already booked for these dates.\n";
        return;
    }
    double price = strategy->calculatePrice(days);

    // Reservation is pending, car status set to Reserved
    reservations->emplace_back(idUpper, username, startDate, endDate, price, "Pending");
    carIt->setStatus("Reserved");
    saveCarsToFile(carsVec);
    cout << "Reservation request submitted. Awaiting admin approval.\n";
    logAction("User " + username + " requested reservation for car ID " + idUpper + " from " + startDate + " to " + endDate + ". Price: $" + to_string(price));

    saveReservationsToFile(*reservations);
}

// ...existing code...
void rentCarWithValidation(User& user, vector<Car>& carsVec) {
    while (true) {
        string carId;
        cout << "Enter Car ID to rent (or 0 to cancel): ";
        getline(cin >> ws, carId);

        if (carId == "0") return;

        if (carId.empty() || carId.find(' ') != string::npos) {
            cout << "Invalid input. Please enter a single Car ID or 0 to cancel.\n";
            continue;
        }

        string idUpper = toUpper(carId);

        // Check if Car ID exists and is available
        auto carIt = find_if(
            carsVec.begin(), carsVec.end(),
            [&](const Car& c) { return toUpper(c.getId()) == idUpper && c.isAvailable(); }
        );
        if (carIt == carsVec.end()) {
            cout << "Car ID not found or not available. Please enter a valid Car ID.\n";
            continue;
        }

        string startDate, endDate;
        int days = 1;

        // Lambda for date validation
        auto isValidDate = [](const string& date) {
            if (date.length() != 10) return false;
            if (date[4] != '-' || date[7] != '-') return false;
            for (int i = 0; i < 10; ++i) {
                if (i == 4 || i == 7) continue;
                if (!isdigit(date[i])) return false;
            }
            return true;
        };

        // Ask for start date until valid
        while (true) {
            cout << "Enter start date: ";
            cin >> startDate;
            if (!isValidDate(startDate)) {
                cout << "Invalid date format. Please use YYYY-MM-DD.\n";
                continue;
            }
            break;
        }

        // Ask for end date until valid and after start date
        while (true) {
            cout << "Enter end date: ";
            cin >> endDate;
            if (!isValidDate(endDate)) {
                cout << "Invalid date format. Please use YYYY-MM-DD.\n";
                continue;
            }

            // Calculate days and check logic
            int days = 1;
            try {
                size_t dash1 = startDate.find('-');
                size_t dash2 = startDate.find('-', dash1 + 1);
                int startYear = stoi(startDate.substr(0, dash1));
                int startMonth = stoi(startDate.substr(dash1 + 1, dash2 - dash1 - 1));
                int startDay = stoi(startDate.substr(dash2 + 1));

                dash1 = endDate.find('-');
                dash2 = endDate.find('-', dash1 + 1);
                int endYear = stoi(endDate.substr(0, dash1));
                int endMonth = stoi(endDate.substr(dash1 + 1, dash2 - dash1 - 1));
                int endDay = stoi(endDate.substr(dash2 + 1));

                days = (endYear - startYear) * 360 + (endMonth - startMonth) * 30 + (endDay - startDay) + 1;
            } catch (...) {
                cout << "Invalid date format. Please use YYYY-MM-DD.\n";
                continue;
            }

            if (days <= 0) {
                cout << "End date must be after start date.\n";
                continue; // ask for end date again
            }
            break; // valid end date
        }

        try {
            StandardPricing strategy;
            if (isReservationConflict(*user.getReservations(), idUpper, startDate, endDate)) {
                cout << "Reservation conflict: Car is already booked for these dates.\n";
                return;
            }
            int days = 1;
            size_t dash1 = startDate.find('-');
            size_t dash2 = startDate.find('-', dash1 + 1);
            int startYear = stoi(startDate.substr(0, dash1));
            int startMonth = stoi(startDate.substr(dash1 + 1, dash2 - dash1 - 1));
            int startDay = stoi(startDate.substr(dash2 + 1));

            dash1 = endDate.find('-');
            dash2 = endDate.find('-', dash1 + 1);
            int endYear = stoi(endDate.substr(0, dash1));
            int endMonth = stoi(endDate.substr(dash1 + 1, dash2 - dash1 - 1));
            int endDay = stoi(endDate.substr(dash2 + 1));

            days = (endYear - startYear) * 360 + (endMonth - startMonth) * 30 + (endDay - startDay) + 1;

            double price = strategy.calculatePrice(days);

            user.getReservations()->emplace_back(idUpper, user.getUsername(), startDate, endDate, price, "Pending");
            carIt->setStatus("Reserved");
            saveCarsToFile(carsVec);
            cout << "Reservation request submitted. Awaiting admin approval.\n";
            user.logAction("User " + user.getUsername() + " requested reservation for car ID " + idUpper + " from " + startDate + " to " + endDate + ". Price: $" + to_string(price));

            saveReservationsToFile(*user.getReservations());
        } catch (const exception& e) {
            cout << e.what() << endl;
        }
        break; // successful rent, exit loop
    }
}


// --- Admin Methods ---
void Admin::addCar(const string& id, const string& model, const string& plateNumber) {
    string idUpper = toUpper(id);
    if (id.empty() || model.empty() || plateNumber.empty()) {
        cout << "Car ID, Model, and Plate Number cannot be empty.\n";
        return;
    }
    for (const auto& car : cars) {
        if (toUpper(car.getId()) == idUpper) {
            cout << "Car ID already exists.\n";
            return;
        }
        if (car.getPlateNumber() == plateNumber) {
            cout << "Plate Number already exists.\n";
            return;
        }
    }
    cars.emplace_back(idUpper, model, plateNumber);
    saveCarsToFile(cars);
    cout << "Car added successfully.\n";
}

void Admin::viewCars() const {
    cout << "\nAll Cars:\n";
    cout << left << setw(15) << "Car ID" << setw(20) << "Model" << setw(15) << "Plate No." << setw(15) << "Status" << endl;
    cout << string(65, '-') << endl;
    for (const auto& car : cars) {
        cout << left << setw(15) << car.getId()
             << setw(20) << car.getModel()
             << setw(15) << car.getPlateNumber()
             << setw(15) << car.getStatus() << endl;
    }
}

void Admin::updateCar(const string& id, const string& newModel) {
    string idUpper = toUpper(id);
    if (newModel.empty()) {
        cout << "Model cannot be empty.\n";
        return;
    }
    for (auto& car : cars) {
        if (toUpper(car.getId()) == idUpper) {
            car.setModel(newModel);
            saveCarsToFile(cars);
            cout << "Car updated successfully.\n";
            return;
        }
    }
    cout << "Car ID not found.\n";
}

void Admin::deleteCar(const string& id) {
    string idUpper = toUpper(id);
    auto it = remove_if(cars.begin(), cars.end(), [&](const Car& c) { return toUpper(c.getId()) == idUpper; });
    if (it != cars.end()) {
        cars.erase(it, cars.end());
        saveCarsToFile(cars);
        cout << "Car deleted successfully.\n";
    } else {
        cout << "Car ID not found.\n";
    }
}

void Admin::filterCarsByModel(const string& keyword) const {
    // Helper lambda to lowercase a string
    auto toLower = [](const string& s) {
        string out = s;
        transform(out.begin(), out.end(), out.begin(), ::tolower);
        return out;
    };

    string keywordLower = toLower(keyword);

    // Always show the table header
    cout << "\nFiltered Cars containing \"" << keyword << "\":\n";
    cout << left << setw(15) << "Car ID" << setw(20) << "Model" << setw(15) << "Plate No." << setw(15) << "Status" << endl;
    cout << string(65, '-') << endl;

    bool found = false;
    for (const auto& car : cars) {
        if (toLower(car.getModel()).find(keywordLower) != string::npos) {
            found = true;
            cout << left << setw(15) << car.getId()
                 << setw(20) << car.getModel()
                 << setw(15) << car.getPlateNumber()
                 << setw(15) << car.getStatus() << endl;
        }
    }
    if (!found) {
        cout << "No cars found matching the filter.\n";
    }
}

void Admin::viewAllReservations() const {
    cout << "\nAll Reservations:\n";
    cout << left << setw(15) << "Car ID" << setw(15) << "Username" << setw(15) << "Start Date"
         << setw(15) << "End Date" << setw(15) << "Price" << setw(15) << "Status" << setw(15) << "Payment" << endl;
    cout << string(105, '-') << endl;
    for (const auto& res : *reservations) { // <-- dereference pointer here
        cout << left << setw(15) << res.getCarId()
             << setw(15) << res.getUsername()
             << setw(15) << res.getStartDate()
             << setw(15) << res.getEndDate()
             << setw(15) << res.getPrice()
             << setw(15) << res.getStatus()
             << setw(15) << res.getPaymentStatus() << endl;
    }
}

void Admin::updateReservationStatus(const string& carId, const string& username, const string& newStatus, vector<Car>& carsVec, vector<Reservation>& reservations) {
    string carIdUpper = toUpper(carId);
    string usernameUpper = toUpper(username);

    // Only accept valid statuses (case-insensitive)
    string statusUpper = toUpper(newStatus);
    if (statusUpper != "PENDING" && statusUpper != "CONFIRMED" && statusUpper != "CANCELLED") {
        cout << "Invalid status. Only Pending, Confirmed, or Cancelled are allowed.\n";
        return;
    }

    // Check if username exists in reservations for this car
    bool userFound = false;
    for (const auto& res : reservations) {
        if (toUpper(res.getCarId()) == carIdUpper && toUpper(res.getUsername()) == usernameUpper) {
            userFound = true;
            break;
        }
    }
    if (!userFound) {
        cout << "Username not found for this Car ID.\n";
        return;
    }

    for (auto& res : reservations) {
        if (toUpper(res.getCarId()) == carIdUpper && toUpper(res.getUsername()) == usernameUpper) {
            res.setStatus(statusUpper[0] + string(statusUpper.begin() + 1, statusUpper.end())); // Capitalize first letter
            // Update car status accordingly
            for (auto& car : carsVec) {
                if (toUpper(car.getId()) == carIdUpper) {
                    if (statusUpper == "CONFIRMED") {
                        car.setStatus("Rented");
                    } else if (statusUpper == "CANCELLED") {
                        car.setStatus("Available");
                    } else if (statusUpper == "PENDING") {
                        car.setStatus("Reserved");
                    }
                    saveCarsToFile(carsVec);
                    break;
                }
            }
            saveReservationsToFile(reservations);
            cout << "Reservation status updated.\n";
            return;
        }
    }
    cout << "Reservation not found.\n";
}

void Admin::viewUsers(const vector<User>& users) const {
    cout << "\nRegistered Users:\n";
    for (const auto& user : users) {
        cout << "- " << user.getUsername() << endl;
    }
}

void Admin::deleteUser(vector<User>& users, const string& username) {
    auto it = remove_if(users.begin(), users.end(), [&](const User& u) { return u.getUsername() == username; });
    if (it != users.end()) {
        users.erase(it, users.end());
        saveUsersToFile(users);
        cout << "User deleted.\n";
    } else {
        cout << "User not found.\n";
    }
}

// --- Reporting Example: Most Rented Car ---
void reportMostRentedCar(const vector<Reservation>& reservations) {
    map<string, int> carCount;
    for (const auto& res : reservations) {
        if (res.getStatus() != "Cancelled")
            carCount[res.getCarId()]++;
    }
    string mostRented;
    int maxCount = 0;
    for (const auto& pair : carCount) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mostRented = pair.first;
        }
    }
    if (!mostRented.empty())
        cout << "Most rented car ID: " << mostRented << " (" << maxCount << " times)\n";
    else
        cout << "No rentals found.\n";
}

int getNumericInputInRange(const string& prompt, int min, int max) {
    string input;
    int value;
    while (true) {
        cout << prompt;
        getline(cin >> ws, input); // Read the whole line, skip leading whitespace
        // Check if input is a single number (no spaces, all digits)
        if (!input.empty() && all_of(input.begin(), input.end(), ::isdigit)) {
            value = stoi(input);
            if (value >= min && value <= max) {
                break;
            } else {
                cout << "Invalid input. Please enter the right number.\n";
            }
        } else {
            cout << "Invalid input. Please enter the right number.\n";
        }
    }
    return value;
}

// --- User Menu with Cancel Reservation and Change Password ---
void userMenu(User& user, vector<Car>& cars, vector<User>& users) {
    int choice;
    do {
        cout << "\nUser Menu:\n1. View Available Cars\n2. Rent Car\n3. View My Reservations\n4. Cancel Reservation\n5. Change Password\n6. Pay for Reservation\n7. Logout\nChoose: ";
        choice = getNumericInputInRange("", 1, 7);
        if (choice == 1) {
            user.viewAvailableCars();
        } else if (choice == 2) {
            user.viewAvailableCars();
            rentCarWithValidation(user, cars);
        } else if (choice == 3) {
            user.viewMyReservations();
        } else if (choice == 4) {
            cancelReservationWithPrompt(user, cars); // Use enhanced cancel flow
        } else if (choice == 5) {
            string newPass;
            cout << "Enter new password: ";
            cin >> newPass;
            user.changePassword(newPass);
            saveUsersToFile(users); // Save after password change
        } else if (choice == 6) {
            user.payForReservation();
        }
    } while (choice != 7);
}

// --- Admin Menu with User Management and Reporting ---
// Now takes cars by reference for syncing
void adminMenu(Admin& admin, vector<User>& users, vector<Reservation>& reservations, vector<Car>& cars) {
    int choice;
    do {
        cout << "\nAdmin Menu:\n1. View Cars\n2. Add Car\n3. Update Car\n4. Delete Car\n5. Filter Cars\n6. View Reservations\n7. Update Reservation Status\n8. View Users\n9. Delete User\n10. Most Rented Car Report\n11. Logout\nChoose: ";
        choice = getNumericInputInRange("", 1, 11);
        if (choice == 1) {
            admin.viewCars();
        } else if (choice == 2) {
            admin.viewCars();
            string id, model, plate;
            cout << "Enter new Car ID: ";
            cin >> id;
            cout << "Enter new Car Model: ";
            cin >> model;
            cout << "Enter Plate Number: ";
            cin >> plate;
            admin.addCar(id, model, plate);
            cars = admin.getCars();
            for (auto& user : users) user.setCars(&cars);
        } else if (choice == 3) {
            admin.viewCars();
            string id, model;
            cout << "Enter Car ID to update: ";
            cin >> id;
            cout << "Enter new Model: ";
            cin >> model;
            admin.updateCar(id, model);
            cars = admin.getCars();
            for (auto& user : users) user.setCars(&cars);
        } else if (choice == 4) {
            while (true) {
                if (admin.getCars().empty()) {
                    cout << "No car to delete.\n";
                    break;
                }
                admin.viewCars();
                string id;
                cout << "Enter Car ID to delete (or 0 to stop deleting): ";
                cin >> id;
                if (id == "0") break;
                admin.deleteCar(id);
                cars = admin.getCars();
                for (auto& user : users) user.setCars(&cars);

                if (admin.getCars().empty()) {
                    cout << "No car to delete.\n";
                    break;
                }
                string more;
                cout << "Do you want to delete another car? (y/n): ";
                cin >> more;
                if (toUpper(more) != "Y") break;
            }
        } else if (choice == 5) {
            string keyword;
            cout << "Enter keyword to filter by model: ";
            cin >> keyword;
            admin.filterCarsByModel(keyword);
        } else if (choice == 6) {
            admin.viewAllReservations();
        // ...existing code...
        }else if (choice == 7) {
    cout << "\nPending Reservation Requests:\n";
    cout << left << setw(15) << "Car ID"
         << setw(15) << "Username"
         << setw(15) << "Start Date"
         << setw(15) << "End Date"
         << setw(15) << "Price"
         << setw(15) << "Status"
         << setw(15) << "Payment" << endl;
    cout << string(105, '-') << endl;
    vector<string> pendingCarIds;
    vector<string> pendingUsernames;
    bool found = false;
    for (const auto& res : reservations) {
        if (res.getStatus() == "Pending") {
            found = true;
            cout << left << setw(15) << res.getCarId()
                 << setw(15) << res.getUsername()
                 << setw(15) << res.getStartDate()
                 << setw(15) << res.getEndDate()
                 << setw(15) << res.getPrice()
                 << setw(15) << res.getStatus()
                 << setw(15) << res.getPaymentStatus() << endl;
            pendingCarIds.push_back(toUpper(res.getCarId()));
            pendingUsernames.push_back(toUpper(res.getUsername()));
        }
    }
    if (!found) {
        cout << "No pending reservation requests.\n";
        continue;
    }
    string carId, username, status;
    // Only accept Car ID that is in pending reservations
    while (true) {
        cout << "Enter Car ID of reservation: ";
        cin >> carId;
        if (find(pendingCarIds.begin(), pendingCarIds.end(), toUpper(carId)) != pendingCarIds.end()) {
            break;
        } else {
            cout << "Car ID not found in pending reservations. Please try again.\n";
        }
    }
    // Only accept Username that matches the Car ID in pending reservations
    while (true) {
        cout << "Enter Username of reservation: ";
        cin >> username;
        bool userFound = false;
        for (const auto& res : reservations) {
            if (toUpper(res.getCarId()) == toUpper(carId) && toUpper(res.getUsername()) == toUpper(username) && res.getStatus() == "Pending") {
                userFound = true;
                break;
            }
        }
        if (userFound) {
            break;
        } else {
            cout << "Username not found for this Car ID in pending reservations. Please try again.\n";
        }
    }
    // Only accept valid status (Pending/Confirmed/Cancelled, case-insensitive)
    while (true) {
        cout << "Enter new status (Pending/Confirmed/Cancelled): ";
        cin >> status;
        string statusUpper = toUpper(status);
        if (statusUpper == "PENDING" || statusUpper == "CONFIRMED" || statusUpper == "CANCELLED") {
            break;
        } else {
            cout << "Invalid status. Only Pending, Confirmed, or Cancelled are allowed.\n";
        }
    }
    admin.updateReservationStatus(carId, username, status, cars, reservations);
    cars = admin.getCars();
    for (auto& user : users) user.setCars(&cars);
}
        else if (choice == 8) {
            admin.viewUsers(users);
        } else if (choice == 9) {
            // Enhanced: Loop for multiple user deletions
            while (true) {
                if (users.empty()) {
                    cout << "No user to delete.\n";
                    break;
                }
                cout << "\nRegistered Users:\n";
                for (const auto& user : users) {
                    cout << "- " << user.getUsername() << endl;
                }
                string username;
                cout << "Enter username to delete (or 0 to stop deleting): ";
                cin >> username;
                if (username == "0") break;
                admin.deleteUser(users, username);

                if (users.empty()) {
                    cout << "No user to delete.\n";
                    break;
                }
                string more;
                cout << "Do you want to delete another user? (y/n): ";
                cin >> more;
                if (toUpper(more) != "Y") break;
            }
        } else if (choice == 10) {
            reportMostRentedCar(reservations);
        }
    } while (choice != 11);
}


class SingletonBase {
public:
    virtual void dummy() = 0; // Pure virtual function
    virtual ~SingletonBase() {}
};

class AdminSingleton : public SingletonBase {
private:
    static AdminSingleton* instance;
    Admin adminInstance;
    AdminSingleton() {} // Private constructor

public:
    // Delete copy constructor and assignment
    AdminSingleton(const AdminSingleton&) = delete;
    AdminSingleton& operator=(const AdminSingleton&) = delete;

    static AdminSingleton* getInstance() {
        if (!instance) {
            instance = new AdminSingleton();
        }
        return instance;
    }

    Admin& getAdmin() { return adminInstance; }

    void dummy() override {} // Implementation of pure virtual
    ~AdminSingleton() {}
};

AdminSingleton* AdminSingleton::instance = nullptr;



int main() {
    vector<Car> cars;
    vector<User> users;
    vector<Reservation> reservations;

    try {
        loadCarsFromFile(cars);
        loadUsersFromFile(users, cars);
        loadReservationsFromFile(reservations);
        ifstream carFile("cars.txt");
        bool isEmpty = carFile.peek() == ifstream::traits_type::eof();
        carFile.close();
        if (isEmpty) {
            cars.push_back(Car("C001", "Toyota_Vios", "ABC123", "Available"));
            cars.push_back(Car("C002", "Honda_Civic", "DEF456", "Available"));
            cars.push_back(Car("C003", "Ford_Ranger", "GHI789", "Available"));
            cars.push_back(Car("C004", "Hyundai_Accent", "JKL012", "Available"));
            cars.push_back(Car("C005", "Mazda_3", "MNO345", "Available"));
            cars.push_back(Car("C006", "Nissan_Almera", "PQR678", "Available"));
            cars.push_back(Car("C007", "Suzuki_Swift", "STU901", "Available"));
            cars.push_back(Car("C008", "Chevrolet_Spark", "VWX234", "Available"));
            cars.push_back(Car("C009", "Kia_Picanto", "YZA567", "Available"));
            cars.push_back(Car("C010", "Mitsubishi_Mirage", "BCD890", "Available"));
            cars.push_back(Car("C011", "Toyota_Fortuner", "EFG123", "Available"));
            cars.push_back(Car("C012", "Honda_CRV", "HIJ456", "Available"));
            cars.push_back(Car("C013", "Ford_Everest", "KLM789", "Available"));
            saveCarsToFile(cars);
        }
        // Use Singleton for Admin
       AdminSingleton* adminSingleton = AdminSingleton::getInstance();
Admin& admin = adminSingleton->getAdmin();
admin.getCars() = cars;
admin.setReservations(&reservations); // <-- Use this line
        for (auto& user : users) {
    	user.setReservations(&reservations);
}

       int mainOption;
do {
    cout << "\nWelcome to Car Rental System\n";
    cout << "1. Login as User\n";
    cout << "2. Login as Admin\n";
    cout << "3. Exit\n";
    mainOption = getNumericInputInRange("Choose: ", 1, 3);

    if (mainOption == 1) {
        int userOption;
        do {
            cout << "\nUser Menu:\n";
            cout << "1. Register\n";
            cout << "2. Login\n";
            cout << "3. Back\n";
            userOption = getNumericInputInRange("Choose: ", 1, 3);

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
                    if (toUpper(user.getUsername()) == toUpper(username) && user.getPassword() == password) {
                        found = true;
                        cout << "Login successful.\n";
                        user.setReservations(&reservations);
                        userMenu(user, cars, users); // Pass users here
                        break;
                    }
                }
                if (!found) {
                    cout << "Invalid credentials.\n";
                    break;
                }
            }
        } while (userOption != 3);


            } else if (mainOption == 2) {
                string adminPass;
                cout << "Enter admin password: ";
                cin >> adminPass;
                if (adminPass == "group2finalproject") {
                    cout << "Admin login successful.\n";
                    adminMenu(admin, users, reservations, cars);
                } else {
                    cout << "Invalid admin password.\n";
                }
            }
        } while (mainOption != 3);

    } catch (const std::exception& ex) {
        cout << "An error occurred: " << ex.what() << endl;
    } catch (...) {
        cout << "An unknown error occurred.\n";
    }

    return 0;
}
