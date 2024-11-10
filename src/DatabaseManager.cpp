#include <set>
#include "DatabaseManager.h"
#include <Poco/Data/PostgreSQL/Connector.h>
#include <Poco/Data/PostgreSQL/PostgreSQL.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/Statement.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <random>

using namespace Poco::Data::Keywords;
using Poco::Data::Statement;
using Poco::Data::Session;

DatabaseManager::DatabaseManager() 
    : sessionPool_(nullptr) {  
    Poco::Data::PostgreSQL::Connector::registerConnector();
}

DatabaseManager::~DatabaseManager() {
    delete sessionPool_;  
    Poco::Data::PostgreSQL::Connector::unregisterConnector();
}

const std::string DatabaseManager::CONNECTION_STRING =
"host=127.0.1.1 port=5433 dbname=taskqueue1 user=yugabyte password=yugabyte";

bool DatabaseManager::init() {
    try {
        std::cout << "\n=== Initializing Database Connection ===\n" << std::endl;
        sessionPool_ = new Poco::Data::SessionPool("PostgreSQL", CONNECTION_STRING, 1, 10, 5);
        Poco::Data::Session session = sessionPool_->get();

        // Test connection
        session << "SELECT 1", now;
        std::cout << "✓ Database connected successfully\n" << std::endl;

        // Create tasks table
        session << "DROP TABLE IF EXISTS tasks", now;
        session << "CREATE TABLE tasks ("
                  "id UUID PRIMARY KEY,"
                  "name VARCHAR(255),"
                  "data TEXT,"
                  "priority INTEGER,"
                  "status VARCHAR(50) DEFAULT 'PENDING',"
                  "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                  "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                  "completed_at TIMESTAMP,"
                  "assigned_worker UUID,"
                  "retry_count INTEGER DEFAULT 0,"
                  "max_retries INTEGER DEFAULT 3"
                  ")", now;

        std::cout << "✓ Tasks table created successfully\n" << std::endl;

        // Add sample tasks
        addSampleTasks();
        return true;

    } catch (const Poco::Exception& exc) {
        std::cerr << "❌ Database initialization error: " << exc.displayText() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "❌ Unknown error occurred during database initialization." << std::endl;
        return false;
    }
}


void DatabaseManager::addSampleTasks() {
    std::vector<std::pair<std::string, std::string>> sampleTasks = {
        {"DataProcessing", "Process customer data batch #1234"},
        {"ImageResizing", "Resize product images for mobile app"},
        {"EmailCampaign", "Send newsletter to subscribers"},
        {"DatabaseBackup", "Perform weekly database backup"},
        {"LogAnalysis", "Analyze system logs for errors"},
        {"ReportGeneration", "Generate monthly sales report"},
        {"UserSync", "Synchronize user data with CRM"},
        {"SecurityScan", "Perform security vulnerability scan"}
    };

    // Create a set to track used priorities
    std::set<int> usedPriorities;
    
    for (const auto& taskData : sampleTasks) {
        Task newTask(taskData.first, taskData.second);
        
        // Generate a unique random priority
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, static_cast<int>(sampleTasks.size()));
        
        int priority;
        do {
            priority = dis(gen);
        } while (usedPriorities.find(priority) != usedPriorities.end());
        
        usedPriorities.insert(priority);
        newTask.setPriority(priority);
        addTask(newTask);
    }
    
    std::cout << "✓ Sample tasks added successfully\n" << std::endl;
}

std::set<int> assignedPriorities;


Task DatabaseManager::getTask(const Poco::UUID& taskId) {
    try {
        Session session = sessionPool_->get();
        std::string id = taskId.toString();
        std::string name, data, status;
        int priority;

        Statement select(session);
        select << "SELECT name, data, status, priority FROM tasks WHERE id = $1",
            bind(id),
            into(name),
            into(data),
            into(status),
            into(priority),
            now;

        Task task(taskId, name, data);
        task.setPriority(priority);
        task.setStatus(status);
        return task;
    }
    catch (const std::exception& e) {
        std::cerr << "Error getting task: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::updateTaskStatus(const Poco::UUID& taskId, const std::string& status) {
    try {
        Session session = sessionPool_->get();
        std::string id = taskId.toString();
        bool completed = (status == "COMPLETED");
        
        std::string idCopy = id;
        std::string statusCopy = status;

        session << "UPDATE tasks SET status = ?, updated_at = CURRENT_TIMESTAMP, "
                  "completed_at = CASE WHEN ? = true THEN CURRENT_TIMESTAMP ELSE NULL END "
                  "WHERE id = ?",
            use(statusCopy),
            use(completed),
            use(idCopy),
            now;

        std::cout << "Task " << id << " status updated to: " << status 
                 << " (completed: " << (completed ? "true" : "false") << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error updating task status: " << e.what() << std::endl;
        throw;
    }
}

std::vector<Task> DatabaseManager::getPendingTasks() {
    try {
        Session session = sessionPool_->get();
        std::vector<Task> tasks;
        
        std::string id, name, data, status;
        int priority;
        
        Statement select(session);
        select << "SELECT id, name, data, status, priority FROM tasks "
                 "WHERE status = 'PENDING' ORDER BY priority DESC, created_at ASC",
            into(id),
            into(name),
            into(data),
            into(status),
            into(priority),
            range(0, 1);

        while (!select.done()) {
            select.execute();
            Task task(Poco::UUID(id), name, data);
            task.setPriority(priority);
            tasks.push_back(task);
        }

        return tasks;
    } catch (const std::exception& e) {
        std::cerr << "Error getting pending tasks: " << e.what() << std::endl;
        throw;
    }
}

// New function to get completed tasks ordered by priority
std::vector<Task> DatabaseManager::getCompletedTasks() {
    try {
        Session session = sessionPool_->get();
        std::vector<Task> tasks;
        
        std::string id, name, data, status;
        int priority;
        
        Statement select(session);
        select << "SELECT id, name, data, status, priority FROM tasks "
                 "WHERE status = 'COMPLETED' ORDER BY priority ASC",
            into(id),
            into(name),
            into(data),
            into(status),
            into(priority),
            range(0, 9);

        while (!select.done()) {
            select.execute();
            Task task(Poco::UUID(id), name, data);
            task.setPriority(priority);
            tasks.push_back(task);
        }

        return tasks;
    } catch (const std::exception& e) {
        std::cerr << "Error getting completed tasks: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::updateTaskAssignment(const Poco::UUID& taskId, const Poco::UUID& workerId, const std::string& status) {
    try {
        Session session = sessionPool_->get();
        std::string id = taskId.toString();
        std::string worker_id = workerId.toString();
        std::string statusCopy = status;
        
        // Get current timestamp
        std::string timestamp = Poco::DateTimeFormatter::format(
            Poco::DateTime(),
            "%Y-%m-%d %H:%M:%S"
        );

        session << "UPDATE tasks SET "
                  "assigned_worker = $1, "
                  "status = $2, "
                  "updated_at = CURRENT_TIMESTAMP, "
                  "completed_at = CASE WHEN $2 = 'COMPLETED' THEN CURRENT_TIMESTAMP ELSE NULL END "
                  "WHERE id = $3",
            use(worker_id),
            use(statusCopy),
            use(id),
            now;

        std::cout << "✓ Task " << id << " assigned to worker " << worker_id 
                  << " with status: " << status << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error updating task assignment: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::markTaskCompleted(const Poco::UUID& taskId, const Poco::UUID& workerId) {
    try {
        Session session = sessionPool_->get();
        std::string id = taskId.toString();
        std::string worker_id = workerId.toString();
        std::string status = "COMPLETED";

        session << "UPDATE tasks SET "
                  "status = $1, "
                  "completed_at = CURRENT_TIMESTAMP, "
                  "updated_at = CURRENT_TIMESTAMP, "
                  "assigned_worker = $2 "
                  "WHERE id = $3",
            use(status),
            use(worker_id),
            use(id),
            now;

        std::cout << "✓ Task " << id << " marked as completed by worker " << worker_id << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error marking task as completed: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::addTask(const Task& task) {
    try {
        Session session = sessionPool_->get();

        std::string id = task.getId().toString();
        std::string name = task.getName();
        std::string data = task.getData();
        std::string status = "PENDING";
        int priority = task.getPriority();
        int retryCount = 0;
        int maxRetries = 3;

        Statement insert(session);
        insert << "INSERT INTO tasks "
                  "(id, name, data, status, priority, retry_count, max_retries) "
                  "VALUES ($1, $2, $3, $4, $5, $6, $7)",
            bind(id),
            bind(name),
            bind(data),
            bind(status),
            bind(priority),
            bind(retryCount),
            bind(maxRetries);

        insert.execute();
        std::cout << "✓ Added task: " << name << " (Priority: " << priority << ")" << std::endl;
    }
    catch (const Poco::Exception& exc) {
        std::cerr << "❌ Error adding task: " << exc.displayText() << std::endl;
        throw;
    }
}



