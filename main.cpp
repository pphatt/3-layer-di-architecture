#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

class Student {
public:
    std::string Name;
    int Age;

    Student(const std::string& name, int age) : Name(name), Age(age) {}
};

class IStudentRepository {
public:
    virtual ~IStudentRepository() = default;
    virtual void AddStudent(const std::string& className, const Student& student) = 0;
    virtual void RemoveStudent(const std::string& className, const std::string& studentName) = 0;
    virtual Student* GetStudentByNameAndClass(const std::string& className, const std::string& studentName) = 0;
    virtual std::vector<Student>* GetAllStudentsByClass(const std::string& className) = 0;
};

class IStudentService {
public:
    virtual ~IStudentService() = default;
    virtual void AddStudentToClass(const std::string& className, const Student& student) = 0;
    virtual void RemoveStudentFromClass(const std::string& className, const std::string& studentName) = 0;
    virtual Student* ViewStudentDetails(const std::string& className, const std::string& studentName) = 0;
    virtual std::vector<Student>* ViewStudentsInClass(const std::string& className) = 0;
};

class StudentRepository : public IStudentRepository {
private:
    std::unordered_map<std::string, std::vector<Student>> _classes;

public:
    void AddStudent(const std::string& className, const Student& student) override {
        _classes[className].push_back(student);
    }

    void RemoveStudent(const std::string& className, const std::string& studentName) override {
        if (_classes.find(className) == _classes.end()) {
            std::cout << className << " class is not found in the system.\n";
            return;
        }

        auto& students = _classes[className];
        auto it = std::remove_if(students.begin(), students.end(),
            [&studentName](const Student& s) { return s.Name == studentName; });

        if (it != students.end()) {
            students.erase(it, students.end());
            std::cout << studentName << " removed from " << className << ".\n";
        } else {
            std::cout << studentName << " not found in " << className << ".\n";
        }
    }

    Student* GetStudentByNameAndClass(const std::string& className, const std::string& studentName) override {
        if (_classes.find(className) == _classes.end()) {
            std::cout << className << " class is not found in the system.\n";
            return nullptr;
        }

        auto& students = _classes[className];
        for (auto& student : students) {
            if (student.Name == studentName) {
                return &student;
            }
        }

        std::cout << studentName << " not found in " << className << ".\n";
        return nullptr;
    }

    std::vector<Student>* GetAllStudentsByClass(const std::string& className) override {
        if (_classes.find(className) == _classes.end()) {
            std::cout << className << " class is not found in the system.\n";
            return nullptr;
        }

        return &_classes[className];
    }
};

class StudentService : public IStudentService {
private:
    std::shared_ptr<IStudentRepository> _repository;

public:
    StudentService(std::shared_ptr<IStudentRepository> repository) : _repository(repository) {}

    void AddStudentToClass(const std::string& className, const Student& student) override {
        _repository->AddStudent(className, student);
    }

    void RemoveStudentFromClass(const std::string& className, const std::string& studentName) override {
        _repository->RemoveStudent(className, studentName);
    }

    Student* ViewStudentDetails(const std::string& className, const std::string& studentName) override {
        return _repository->GetStudentByNameAndClass(className, studentName);
    }

    std::vector<Student>* ViewStudentsInClass(const std::string& className) override {
        return _repository->GetAllStudentsByClass(className);
    }
};

class StudentController {
private:
    std::shared_ptr<IStudentService> _service;

public:
    StudentController(std::shared_ptr<IStudentService> service) : _service(service) {}

    void AddStudent(const std::string& className, const Student& student) {
        _service->AddStudentToClass(className, student);
    }

    void RemoveStudent(const std::string& className, const std::string& studentName) {
        _service->RemoveStudentFromClass(className, studentName);
    }

    std::vector<Student>* GetStudentsByClass(const std::string& className) {
        return _service->ViewStudentsInClass(className);
    }

    Student* GetStudentDetails(const std::string& className, const std::string& studentName) {
        return _service->ViewStudentDetails(className, studentName);
    }
};

class StudentPresentation {
private:
    std::shared_ptr<StudentController> _controller;

public:
    StudentPresentation(std::shared_ptr<StudentController> controller) : _controller(controller) {}

    void InitMenu() {
        std::cout << "Welcome to the Student Management Console\n";
        std::cout << "Options: Add, Remove, View, View Details, Exit\n";

        while (true) {
            std::cout << "\nEnter your choice: ";
            std::string choice;
            std::getline(std::cin, choice);

            if (choice == "Add") {
                AddStudent();
            } else if (choice == "Remove") {
                RemoveStudent();
            } else if (choice == "View") {
                ViewStudents();
            } else if (choice == "View Details") {
                ViewStudentDetails();
            } else if (choice == "Exit") {
                std::cout << "Exiting... Goodbye!\n";
                break;
            } else {
                std::cout << "Invalid choice. Please try again.\n";
            }
        }
    }

private:
    void AddStudent() {
        std::string className, studentName;
        int age;

        std::cout << "Enter class name (e.g., A1): ";
        std::getline(std::cin, className);

        std::cout << "Enter student name: ";
        std::getline(std::cin, studentName);

        std::cout << "Enter student age: ";
        std::cin >> age;
        std::cin.ignore(); // Clear the newline character

        _controller->AddStudent(className, Student(studentName, age));
        std::cout << "Student added successfully!\n";
    }

    void RemoveStudent() {
        std::string className, studentName;

        std::cout << "Enter class name (e.g., A1): ";
        std::getline(std::cin, className);

        std::cout << "Enter student name to remove: ";
        std::getline(std::cin, studentName);

        _controller->RemoveStudent(className, studentName);
    }

    void ViewStudents() {
        std::string className;

        std::cout << "Enter class name (e.g., A1): ";
        std::getline(std::cin, className);

        auto students = _controller->GetStudentsByClass(className);
        if (students) {
            std::cout << "Students in " << className << ":\n";
            for (const auto& student : *students) {
                std::cout << "- " << student.Name << ", Age: " << student.Age << "\n";
            }
        } else {
            std::cout << "No students found in this class.\n";
        }
    }

    void ViewStudentDetails() {
        std::string className, studentName;

        std::cout << "Enter class name (e.g., A1): ";
        std::getline(std::cin, className);

        std::cout << "Enter student name: ";
        std::getline(std::cin, studentName);

        auto student = _controller->GetStudentDetails(className, studentName);
        if (student) {
            std::cout << "Student Details: " << student->Name << ", Age: " << student->Age << "\n";
        } else {
            std::cout << "No student found with the given details.\n";
        }
    }
};

int main() {
    // Dependency Injection Setup
    auto repository = std::make_shared<StudentRepository>();
    auto service = std::make_shared<StudentService>(repository);
    auto controller = std::make_shared<StudentController>(service);
    StudentPresentation presentation(controller);

    presentation.InitMenu();

    return 0;
}