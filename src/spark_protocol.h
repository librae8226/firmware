#include "coap.h"
#include "tropicssl/aes.h"

namespace ChunkReceivedCode {
  enum Enum {
    OK = 0x44,
    BAD = 0x80
  };
}

class SparkProtocol
{
  public:
    int init(const unsigned char *private_key,
             const unsigned char *pubkey,
             const unsigned char *encrypted_credentials,
             const unsigned char *signature);
    CoAPMessageType::Enum received_message(unsigned char *buf);
    void hello(unsigned char *buf);
    void hello(unsigned char *buf, unsigned char token);
    void function_return(unsigned char *buf, unsigned char token);
    void function_return(unsigned char *buf, unsigned char token,
                         bool return_value);
    void function_return(unsigned char *buf, unsigned char token,
                         int return_value);
    void function_return(unsigned char *buf, unsigned char token,
                         double return_value);
    void function_return(unsigned char *buf, unsigned char token,
                         const void *return_value, int length);
    void variable_value(unsigned char *buf, unsigned char token,
                        bool return_value);
    void variable_value(unsigned char *buf, unsigned char token,
                        int return_value);
    void variable_value(unsigned char *buf, unsigned char token,
                        double return_value);
    void variable_value(unsigned char *buf, unsigned char token,
                        const void *return_value, int length);
    void event(unsigned char *buf,
               const char *event_name,
               int event_name_length);
    void event(unsigned char *buf,
               const char *event_name,
               int event_name_length,
               const char *data,
               int data_length);
    void chunk_received(unsigned char *buf, unsigned char token,
                        ChunkReceivedCode::Enum code);
    void update_ready(unsigned char *buf, unsigned char token);

  private:
    aes_context aes;
    unsigned char key[16];
    unsigned char iv[16];
    unsigned char salt[8];
    unsigned short _message_id;

    unsigned short next_message_id();
    void encrypt(unsigned char *buf, int length);
    void separate_response(unsigned char *buf, unsigned char token, unsigned char code);
};
