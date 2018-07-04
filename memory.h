#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <queue>
#include <cmath>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <unordered_set>
#include <cmath>

using namespace std;

/*
 Memory Size: 32KB => 2^15 B
 Bank: 8KB => 2^13 B , 4 banks
 Word: 4B => 2^2 x 1B => 2^13 / 2^2 => 2^11 words / Bank
 Word Count: 2^13 / 2^2  = 2^11 = 2048
 First 2 Bit  of address is used for bank detection
*/

/*
 Initialization File:
   <address:size_t>
 Test File:
   <address:size_t> <access mode:boolean> <data:multiple chars>

 Exection:./a.out <Memory Size in KB> <Bank Count> <Address Initialization File> <Test File>
 */
#define WORD_SIZE 4;
static bool memory_bank_initialize = 0;

class word{
    char _byte[4];
    size_t _address; // Should be of address of 4 bytes
public:
    word(){cout<<"Default Word Constructor"<<endl;}
    ~word(){/*cout<<"WORD DESTRUCTOR"<<endl;*/};
    word(size_t & w_address):_address(w_address)
    {
        for( int index=0; index < 4; index++)
        {
          _byte[index] = '-';
        }
    }
    void createWord(size_t &);
    
    // Get address
    size_t & getAddress()
    {
        return _address;
    };

    // Send value from a address
    std::string readBytes(size_t start_byte_index, size_t end_byte_index)
    { 
      //cout<<"DEBUG:READ:"<<endl;
      std::string bytes;
      while( start_byte_index <= end_byte_index)
      {
        //cout<<"DEBUG:"<<_byte[start_byte_index]<<":"<<endl;
        bytes += _byte[start_byte_index];
        start_byte_index++;
      }
      //cout<<">>DEBUG:WORD-R:"<< bytes<<":"<<endl;
      return bytes;
    }
    // Store multiple char in bytes
    void writeBytes( std::string& value, size_t start_index)
    {
         size_t index = 0;
         while( index < value.length())
         {
           _byte[start_index] = value.at(index);
           index++; 
           start_index++;
         }
    }
 
    // Get values
    std::string getData()
    {
       std::string s;
       for(size_t index =0; index < 4; index++)
       {
         s+=_byte[index];
       } 
       return s;
    }
    
};



/*
 Function Name: createWord
 Params       :
 Returns      : None
 */
void word::createWord(size_t & address)
{
    _address = address;
}



/*
 Class Name: Request
 Description: Create a Request to be passed to the bank/memory controller
 */
class Request
{
    const size_t _address;
    const bool _read_write_access;
    std::string _write_data;  // Used for write operation
    size_t _read_bytes; // Number of bytes to be read
public:
    Request():_address(0),_read_write_access(0){ cout<<"Request Cons"<<endl;}
    ~Request(){/*cout<<"Request Destructor"<<endl;*/};
    Request(size_t address, bool rw_access):_address(address),_read_write_access(rw_access)
    {
    }
    // Used for write access
    void addData(std::string& data) {_write_data = data; }
    // Used for read access
    void addReadBytes(size_t read_bytes) {_read_bytes = read_bytes;}
    
    
    // Address
    size_t getAddress() { return _address;}
    // Get read write access
    bool getReadWriteAccess(){ return _read_write_access;}
    // Get data to be written to the mmory
    std::string& getWriteData(){ return _write_data;}
    // Get number of bytes to be read
    size_t getReadBytes(){ return _read_bytes;}
    
    void display()
    {
      cout<<" Address:"<<_address<<endl;
      cout<<" Access :"<<_read_write_access<<endl;
      if ( _read_write_access )
      { 
        cout<<" Data   :"<<_write_data<<endl;
      }
      else
      {
        cout<<" Byte Number:"<<_read_bytes<<endl;
      }
      
    }
    
};

/*
 Class Name: Response
 Description: Response from read/write
     Read- Only Acknowledgment
     Write- Acknowledgment and Data
 */
class Response
{
    bool _rw_access; // read write access
    size_t _address; // address requested for
    std::string _data; // Reponse data
    size_t _b_index; // To be removed 
public:
    Response(){cout<<"Response Cons"<<endl;}
    ~Response(){/*cout<<"Response Destructor"<<endl;*/}
    Response(size_t address,bool rw_access, std::string& data): _address(address),_rw_access(rw_access),_data(data){};
    Response(size_t address,bool rw_access, std::string& data,size_t b_index): _address(address),_rw_access(rw_access),_data(data){_b_index = b_index;};
    Response(size_t address,bool rw_access): _address(address),_rw_access(rw_access){};
    Response(size_t address,bool rw_access, size_t b_index): _address(address),_rw_access(rw_access){_b_index = b_index;};
    bool getAccessMode(){ return _rw_access;}
    
    void display()
    {
      cout<<" Address:(BI-"<<_b_index<<" : ADD-"<<_address<<")"<<endl;
      cout<<" Access :"<<_rw_access<<endl;
      if (! _rw_access ) 
      {
        cout<<" Data   :"<<_data<<endl;
      }
    }

};

/*
 Class Name: Bank
 Description:Bank memory is consicutive. Words are stored in consicutive location
*/

class Bank
{
    std::vector<word> _words;
    const size_t _bank_index;
public:
    Bank():_bank_index(0){cout<<"Bank- Default Cons"<<endl;}
    ~Bank(){/*cout<<"Bank- Destructor"<<endl;*/};
    Bank(size_t b_index):_bank_index(b_index)
    {
        cout<<"INFO: Bank-"<<b_index<<" has been created"<<endl;
    }

    void initialize(std::vector<size_t>& addressList);
    
    // Read Address
    Response read(size_t address, size_t byte_length);
    
    // Write to address
    Response write(size_t address, std::string& write_data);
   
    // Get bank index
    size_t getBankIndex() { return _bank_index;}
    // Temporary display
    void display()
    {
        for(auto& add: _words)
        {
            cout<<"ADDRESS:"<< add.getAddress()<<":";
            cout<<"DATA:"<< add.getData()<<endl;
        }
    }

};

/*
Function Name: initialize
Parms        : <vector<size_t> stream of address>
Description  : Initialize Bank Memory in cosicutive memory location.
*/
void Bank::initialize(std::vector<size_t>& addressList)
{
    try {
        cout<<"INFO: Loading address for the bank-"<<_bank_index<<endl;
        // First byte address of word
        for( auto& w_address : addressList)
        {
            _words.push_back(w_address);
            //cout<<"Address:"<< _words[_words.size() -1].getAddress()<<",";
            //cout<<"Data:"<< _words[_words.size() -1].getData()<<endl;
        }
    } catch (exception e) {
        cout<<"EXCEPTION: Not able to load address to the bank-"<<_bank_index<<endl;
        cout<<"         : "<<e.what()<<endl;
    }
}


/*
 Function Name: read
 Params       : <size_t>:Memory Address, <size_t>: Number of bytes to be read
 Description  : Read bytes from a specific memory location
*/
Response Bank::read(size_t address, size_t byte_length)
{
    cout<<"INFO: MEMORY READ : Reading \""<< byte_length <<"\" bytes from the Bank:"<<_bank_index<<" , Address:"<<address<<endl;
  // Findout word index 
  size_t starting_word_index = address / 4;
  size_t end_word_index = ( address + byte_length -1) / 4;
  //cout<<">>DEBUG: READ-StartIndex-"<< starting_word_index <<",EndIndex:"<<end_word_index<<",DataSIze:"<<byte_length<< endl;

  std::string bytes;
  size_t start_byte_index = address % 4;
  size_t end_byte_index = 3;
  size_t byte_read_count = byte_length;
  // Read multiple words if required
  while( starting_word_index <= end_word_index )
  {
      
     if ( (address + byte_length -1) < ((starting_word_index + 1) * 4) )
     {
       end_byte_index = start_byte_index + byte_read_count - 1;
     }
     else
     {
       end_byte_index = 3; // Considering word has 4 bytes only.
     }
     //cout<<"WI:"<<starting_word_index<<", SI:"<<start_byte_index <<",EI:"<<end_byte_index<<endl;;
     bytes.append(_words[starting_word_index].readBytes(start_byte_index,end_byte_index ));
     byte_read_count -= end_byte_index - start_byte_index + 1; // remaining byte
     starting_word_index++; // move to the next word 
     start_byte_index = 0;
  }
  
  //return  Response( address,0, bytes);
  return  Response( address,0, bytes,_bank_index);
}   

/*
  Function Name: write
  Params       : <size_t>: Memory Address, <string>: Data to be written
  Description  : Writes the data to the specified memory location
*/
Response Bank::write(size_t address, std::string& data)
{

    cout<<"INFO: MEMORY WRITE : Writing \""<< data <<"\" to the Bank:"<<_bank_index<<", Address:"<<address<<endl;
  size_t byte_length = data.length();
  
   // Findout word index 
  size_t starting_word_index = address / 4;
  //cout<<"DEBUG: WRITE- word index "<< starting_word_index << "DataSize-"<<byte_length<< endl;
  size_t end_word_index = ( address + byte_length -1 ) / 4;

  size_t start_byte_index = address % 4;
  size_t end_byte_index = 3;
  size_t data_index = 0;
  size_t byte_read_count = byte_length;
  // Read multiple words if required
  while( starting_word_index <= end_word_index )
  {
      //cout<<"ByteLen-"<<byte_length<<endl;
     if ( (address + byte_length -1) < (starting_word_index + 1) * 4)
     {
       end_byte_index = start_byte_index + byte_read_count -1;
     }
     else
     {
       end_byte_index = 3; // Considering word has 4 bytes only.
     }
     //cout<<"WI-"<<starting_word_index<<":S-"<<start_byte_index<<":E-"<<end_byte_index<<endl;
     std::string subpart = data.substr(data_index, (end_byte_index - start_byte_index + 1));
     //cout<<">>DEBUG:WI:"<<starting_word_index<<", SI:"<<start_byte_index <<",EI:"<<end_byte_index;;
     //cout<<" ,Data:"<<subpart<<", data_index:"<<data_index<<endl;
     _words[starting_word_index].writeBytes(subpart, start_byte_index );
     data_index += end_byte_index - start_byte_index + 1;
     byte_read_count -= end_byte_index - start_byte_index + 1; // Remaining Byte for read
     //cout<<"ByteLen-"<<byte_length<<endl;
     starting_word_index++;
     start_byte_index = 0;
  }
  
  Response res( address,1, _bank_index);
  //res.display();
  return res; 
}

/*
Class Name : MemoryController
Description: Initialize the memory, Maintain there queues to process request and response.
*/

class MemoryController
{
    // Queue up request
    std::queue<Request> request_queue;
    std::queue<Response>respose_queue;
    std::queue<Request> request_queue_wait; // Enqueue request if there is any bank conflict.
    std::vector<Bank> mem_banks;
    size_t n_bit_msb;
    size_t n_bits_right_shift =10; // Considering KB =10 bits
    const size_t _memory_size = 0;
    const size_t _bank_counts = 0;
    bool _request_handler_flag = 1;
    std::unordered_set<size_t> enqueued_bank_index_set; // used for bank cnflict detection
    
    
  public:
    MemoryController(){cout<<"Memory"<<endl;}
    ~MemoryController()
     {
       //stopRequestHandler();
       cout<<"INFO:Destroying MemoryController"<<endl;
     }
    
    MemoryController(size_t mem_size, size_t bank_counts):_memory_size(mem_size), _bank_counts(bank_counts)
    {
      cout<<"INFO: Memory Controller with Memory:"<<_memory_size<<" KB and" <<_bank_counts<<" Banks"<<endl;
      n_bits_right_shift += log2(_memory_size / _bank_counts);
      //cout<<"INFO: n_bits_right_shift:"<<n_bits_right_shift<<endl;
    }
    // Create Memory Bank
    void initializeMemoryBank(std::string& file_name );
    
    // Get Memory
    std::vector<Bank>& getMemory()
    {
        return mem_banks;
    }
    
    // Request handler , continiously check if there is any request in queue.
    void requestHandler(size_t delay= 10);
    // Check the response queue
    void responseHandler();
    
    // Stop request handler falg
    void stopRequestHandler(){ _request_handler_flag = 0;}
   
    // Process stream of address for Testing
    void processTest(std::string& test_file);
    
    // Check Bank Conflict
    bool checkBankConflict(size_t address)
    {
        size_t bank_index = getBankIndex(address, n_bits_right_shift);
        if( enqueued_bank_index_set.find(bank_index) == enqueued_bank_index_set.end())
        {
            enqueued_bank_index_set.insert(bank_index);
            return false;
        }
        cout<<"INFO: BANK CONFLICT, Bank:"<<bank_index<<" , Address:"<<address<<endl;
        return true;
    }
    // Update enqueed_address_set
    void updateEnqueuedSet(size_t address)
    {
        size_t bank_index = getBankIndex(address, n_bits_right_shift);
        if( enqueued_bank_index_set.find(bank_index) != enqueued_bank_index_set.end())
        {
            enqueued_bank_index_set.erase(enqueued_bank_index_set.find(bank_index)); // Remove address from set
        }
    }
    // Update request queue with one new unique request.
    void updateRequestQueue()
    {
        // Enqueue a new request to request_queue from wait queue without any bank conflict
        bool isEnqueued =  true;
        size_t queue_elem_index = 0;
        while(! request_queue_wait.empty() && isEnqueued)
        {
            Request& req = request_queue_wait.front();
            request_queue_wait.pop();
            size_t address = req.getAddress();
            queue_elem_index++; 
            if(checkBankConflict(address))
            { // bank conflict
                request_queue_wait.push(req);
                if ( queue_elem_index == request_queue_wait.size())
                {
                  isEnqueued = false;
                }
            }
            else
            {
                request_queue.push(req); // Enqueue to the request
                isEnqueued = false;
            } 
        }
    }
   
    // Calculate Bank Index from a memory address 
    size_t getBankIndex(size_t address, size_t right_shift_bits )
    {
        size_t bank_index = address >> right_shift_bits;
        return bank_index;
    }
    // Temporary Display
    void display(size_t bank_count);
    
};

/*
 Function Name: requestHandler()
 Params       :
 Retruns      : None
 Description  : Checks the Request Queue and process that request
 - Request Types ( Read , Write )
 -
 */
void MemoryController::requestHandler(size_t delay)
{
    cout<<"INFO: Request Handler:"<<endl;
    while(! request_queue.empty())
    {
        Request req(request_queue.front());
        request_queue.pop();
        size_t address = req.getAddress();
        
        // Check the bank.
        size_t bank_index = getBankIndex(address, n_bits_right_shift);
        // Access mode
        bool rw_access_mode =  req.getReadWriteAccess();
        
        if ( rw_access_mode)
        {
           respose_queue.push(mem_banks[bank_index].write(address, req.getWriteData()));
        }
        else
        {
           // Read: Send the request to the corresponding bank
           respose_queue.push(mem_banks[bank_index].read(address, req.getReadBytes()));
        }
        // Update enqueued_address_set
        updateEnqueuedSet(address);
        
        // Update wait_request_queue
        updateRequestQueue();
    }
    cout<<"INFO: All request finished"<<endl;
}

/*
 Function Name: responseHandler()
 Params       :
 Retruns      : None
 Description  : Checks the Request Queue and process that request
 - Request Types ( Read , Write )
 -
 */
void MemoryController::responseHandler()
{
    cout<<"INFO: Response Handler:"<<endl;
    while(! respose_queue.empty())
    {
        Response res( respose_queue.front());
        // Print the information
        cout<<"Response:************************"<<endl;
        res.display();
        respose_queue.pop();
    }
}

/*
 Function Name:processTest()
 Params       :
 Description: Create a memory access request and enqueue it to the memory controller Request Queue.
 */
void MemoryController::processTest(std::string &test_file)
{
    try {
        // Read from address input file
        ifstream fObj;
        fObj.open(test_file);
        if(fObj.is_open())
        {
            std::string line;
            // Read each line from test file
            while( std::getline(fObj, line))
            {
                std::istringstream iss(line);
                size_t address;
                bool rw_access;
                std::string data;
                size_t read_byte_count;
                iss>> address;
                iss>> rw_access;
                Request req(address, rw_access);
                // rw_access== 1 , write request
                if( rw_access)
                {
                    iss>> data;
                    req.addData(data);
                }
                else{  // rw_access = 0 read request
                    iss>> read_byte_count;
                    req.addReadBytes(read_byte_count);
                }
                
                // Check Bank Conflict
                if( checkBankConflict(address))
                {
                    request_queue_wait.push(req);
                }
                else
                {
                    // Enqueue the request to Memory Controller Queue
                    request_queue.push(req);
                }
            }
            cout<<"DEBUG: STREAM OF ADDRESS PROCESSING OVER"<<endl;
        }
    } catch (exception& e) {
        cout<<"EXCEPTION:"<<e.what()<<endl;
    }
    
}

/*
  Function Name: initializeMemoryBank
  Prams        :std::<string> -> File Name / List of address to be initialized in memory.
  Retrun       : None
  Description  : Initialize the memory bank.              
*/
void MemoryController::initializeMemoryBank(std::string& file_name)
{
    
    std::map<size_t, std::vector<size_t>> addressListMap;
    size_t bankIndex =0;
    
    size_t word_counts = pow(2,n_bits_right_shift)/WORD_SIZE;
    //cout<<"Word Counts:"<< word_counts<<":"<< n_bits_right_shift<<endl;
    try {
        // Read from address input file
        ifstream fObj;
        fObj.open(file_name);
        if(fObj.is_open())
        {
            size_t w_address;
            size_t w_index = 0; // word address count
            while( fObj >> w_address)
            {
                if(w_index >= word_counts)
                {
                    bankIndex++;
                    w_index = 0;
                }
                addressListMap[bankIndex].push_back(w_address);
                w_index++;
            }
        }
    } catch (exception& e) {
        cout<<"EXCEPTION:"<<e.what()<<endl;
    }
    // Initialize address for Word in a Bank
    for( size_t index = 0; index < 4; index++)
    {
        Bank newBank(index);
        newBank.initialize(addressListMap[index]);
        mem_banks.push_back(newBank);
    }
}


void MemoryController::display(size_t bank_count)
{
    // Display Bank Address
    for( size_t index =0; index < bank_count; index++)
    {
       cout<<"Bank Index-"<<index<<":"<<endl;
       mem_banks[index].display();
    }

}


