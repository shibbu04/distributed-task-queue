#include <gtest/gtest.h>
#include "TaskQueue.h"
#include "LoadBalancer.h"
#include "Task.h"

class TaskQueueTest : public ::testing::Test {
protected:
    TaskQueue taskQueue;
    LoadBalancer loadBalancer;
};

TEST_F(TaskQueueTest, AddTask) {
    Task task("test_task", "test_data");
    taskQueue.addTask(task);
    EXPECT_TRUE(taskQueue.hasTask());
}

TEST_F(TaskQueueTest, GetNextTask) {
    Task task("test_task", "test_data");
    taskQueue.addTask(task);
    Task nextTask = taskQueue.getNextTask();
    EXPECT_EQ(nextTask.getName(), "test_task");
    EXPECT_EQ(nextTask.getData(), "test_data");
}

TEST_F(TaskQueueTest, LoadBalancer) {
    Worker worker("localhost", 8081);
    loadBalancer.addWorker(worker);
    Worker* nextWorker = loadBalancer.getNextAvailableWorker();
    ASSERT_NE(nextWorker, nullptr);
    EXPECT_EQ(nextWorker->getAddress().port(), 8081);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}