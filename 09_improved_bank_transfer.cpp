#include <string>
#include <mutex>
#include <set>
#include <iostream>
#include <thread>
using namespace std;

class Account
{
public:
    Account(string name, double money) : mName(name), mMoney(money) {}

public:
    void changeMoney(double amount)
    {
        mMoney += amount;
    }

    string getName()
    {
        return mName;
    }

    double getMoney()
    {
        return mMoney;
    }

    mutex *getLock()
    {
        return &mMoneyLock;
    }

private:
    string mName;
    double mMoney;
    mutex mMoneyLock;
};

class Bank
{
public:
    void addAccount(Account *account)
    {
        mAccounts.insert(account);
    }

    bool transferMoney(Account *accountA, Account *accountB, double amount)
    {
        lock(*accountA->getLock(), *accountB->getLock());  //这里通过lock函数来获取两把锁，标准库的实现会保证不会发生死锁
        lock_guard guardA(*accountA->getLock(), adopt_lock); // 在修改每个账号之前，需要获取相应的锁
        lock_guard guardB(*accountB->getLock(), adopt_lock);

        if (amount > accountA->getMoney())
        {
            return false;
        }

        accountA->changeMoney(-amount);
        accountB->changeMoney(amount);
        return true;
    }

    double totalMoney() const
    {
        double sum = 0;
        for (auto a : mAccounts)
        {
            sum += a->getMoney();
        }
        return sum;
    }

private:
    set<Account *> mAccounts;
};

mutex sCoutLock;
void randomTransfer(Bank *bank, Account *accountA, Account *accountB)
{
    while (true)
    {
        double randomMoney = ((double)rand() / RAND_MAX) * 100;
        if (bank->transferMoney(accountA, accountB, randomMoney))
        {
            sCoutLock.lock();
            cout << "Transfer " << randomMoney << " from " << accountA->getName()
                 << " to " << accountB->getName()
                 << ", Bank totalMoney: " << bank->totalMoney() << endl;
            sCoutLock.unlock();
        }
        else
        {
            sCoutLock.lock();
            cout << "Transfer failed, "
                 << accountA->getName() << " has only $" << accountA->getMoney() << ", but "
                 << randomMoney << " required" << endl;
            sCoutLock.unlock();
        }
    }
}

int main()
{
    Account a("Paul", 100);
    Account b("Moira", 100);

    Bank aBank;
    aBank.addAccount(&a);
    aBank.addAccount(&b);

    thread t1(randomTransfer, &aBank, &a, &b);
    thread t2(randomTransfer, &aBank, &b, &a);

    t1.join();
    t2.join();

    return 0;
}