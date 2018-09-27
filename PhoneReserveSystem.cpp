class PhoneNumberGiver
{
    unordered_set<string> available;
    unordered_set<string> reserved;
    unordered_map<string, customerID> assigned;
    
    Reserve(const string & number)
    {
        reserved.emplace(str);
    }
    
public:
    vector<string> Reserve(int n)
    {
        vector<string> result;
        for(size_t i = 0, auto it = available.begin() ; it != available.end() && i < n ; i++, it++)
        {
            result.push_back(*it);
        }
        for(auto & number : result)
        {
            available.erase(number);
            Reserve(number);
        }
        return result;
    }
    
    void Assign(const string & number)
    {
        auto it = reserve.find(number);
        if(it != reserve.end())
        {
            assigned.emplace(number);
        }
        else
            throw std::exception();
    }
    
    void ReturnNumber(const vector<string> & number)
    {
        for(auto & s : number)
        {
            reserved.erase(s);
        }
    }
};