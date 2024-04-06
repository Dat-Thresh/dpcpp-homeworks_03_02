#include <iostream>
#include <vector>
#include <fstream>
#include <exception>
#include <memory>

class Observer {
public:
	Observer() = default;
	virtual void onWarning(const std::string& message) {};
	virtual void onError(const std::string& message) {};
	virtual void onFatalError(const std::string& message) {};
	virtual ~Observer() = default;

};

class WarningObserver : public Observer{
public:
	void onWarning(const std::string& message) override {
		std::cout << "It's warning message" << message << std::endl;
	}

};

class ErrorObserver :public Observer {
public:
	void onError(const std::string& message) override {
		std::ofstream out;
		out.open("..\\ErrorLog.txt");
		if (out.is_open()) {
			out << "It's error message" << message << std::endl;
			out.close();
		}
		else {
			std::cout << "Unable to open ErrorLog!" << std::endl;
		}
	}
};

class FatalErrorObserver : public Observer {
public:
	void onFatalError(const std::string& message) override {
		std::ofstream out;
		std::cout << "It's Fatal err message" << message << std::endl;
		if (out.is_open()) {
			out << message << std::endl;
			out.close();
		}
		else {
			std::cout << "Unable to open ErrorLog!" << std::endl;
		}
	}
};

class Observed {
public:
	void warning(const std::string& message) const {
		for (auto el : observers_) {
			if (auto ptr = el.lock()) {
				ptr->onWarning(message);
			}
			else {
				throw std::bad_weak_ptr();
			}
		}
	};
	void error(const std::string& message) const {
		for (auto el : observers_) {
			if (auto ptr = el.lock()) {
				ptr->onError(message);
			}
			else {
				throw std::bad_weak_ptr();
			}
		}
	};
	void fatalError(const std::string& message) const {
		for (auto el : observers_) {
			if (auto ptr = el.lock()) {
				ptr->onFatalError(message);
			}
			else {
				throw std::bad_weak_ptr();
			}
		}
	};

	void addObserver(std::weak_ptr<Observer> observer) {
		observers_.push_back(observer);
	}

protected:
	std::vector <std::weak_ptr<Observer>>observers_;
};

int main() {
	try {
		Observed observed;
		auto WarningObs = std::make_shared<Observer>(new WarningObserver);
		auto ErrorObs = std::make_shared<Observer>(new ErrorObserver);
		auto FatalErObs= std::make_shared<Observer>(new FatalErrorObserver);

		observed.addObserver(WarningObs);
		observed.addObserver(ErrorObs);
		observed.addObserver(FatalErObs);

		observed.warning("Warning!");
		observed.error("Error");
		observed.fatalError("Fatal error");
	}
	catch (std::exception& er) {
		std::cout << er.what();
	}
	return 0;
}