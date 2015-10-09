
#ifdef  _USE_SSL_

#include <string.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include "AESBufferFilter.h"

#include "madara/utility/Utility.h"
#include "madara/utility/LogMacros.h"

Madara::Filters::AESBufferFilter::AESBufferFilter ()
  : key_ (new unsigned char[32]), iv_ (new unsigned char[16])
{
  memset ((void *)key_.get_ptr (), 0, 32);
  memset ((void *)iv_.get_ptr (), 0, 16);
}

Madara::Filters::AESBufferFilter::AESBufferFilter (
  const AESBufferFilter & input)
  : key_ (new unsigned char[32]), iv_ (new unsigned char[16])
{
  memcpy (key_.get_ptr (), input.key_.get_ptr (), 32);
  memset ((void *)iv_.get_ptr (), 0, 16);
}

Madara::Filters::AESBufferFilter::AESBufferFilter (
  unsigned char * key, int key_length)
: key_ (new unsigned char[32]), iv_ (new unsigned char[16])
{
  memcpy (key_.get_ptr (), key, 32);
  memset ((void *)iv_.get_ptr (), 0, 16);
}

Madara::Filters::AESBufferFilter::~AESBufferFilter ()
{
}

int Madara::Filters::AESBufferFilter::generate_key (
  const std::string & password)
{
  int i, rounds = 5;

  // use a random salt and a zero'd initialization vector
  int64_t salt = 0;

  i = EVPBytesToKey (EVP_aes_256_cbc (), EVP_sha1 (),
    (unsigned char *)&salt, (unsigned char *)password.c_str (),
    (int)password.length (), rounds,
    (unsigned char *)key_.get_ptr (),
    (unsigned char *)iv_.get_ptr ());
  if (i != 32)
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      DLINFO "%s:" \
      " Unable to initialized 256 bit AES. Only received %d bytes.\n",
      i));
    return -1;
  }

  return 0;
}


int
Madara::Filters::AESBufferFilter::encode (
  unsigned char * source, int size, int max_size) const
{
  int result_length (0);

  /// encryption context
  EVP_CIPHER_CTX encrypt_context;

  int end_write (size + AES_BLOCK_SIZE);
  int pad_bytes (0);

  // initialize the context
  EVP_CIPHER_CTX_init (&encrypt_context);
  EVP_EncryptInit_ex (&encrypt_context, EVP_aes_256_cbc (), NULL,
    key_.get_ptr (), iv_.get_ptr ());
  //EVP_EncryptInit_ex (&encrypt_context, NULL, NULL, NULL, NULL);

  EVPEncryptUpdate (&encrypt_context,
    source, &end_write,
    source, size);
  EVP_EncryptFinal_ex (&encrypt_context, source + end_write, &pad_bytes);

  result_length = end_write + pad_bytes;

  return result_length;
}

int
Madara::Filters::AESBufferFilter::decode (
  unsigned char * source, int size, int max_size) const
{
  int result_length (0);

  /// decryption context
  EVP_CIPHER_CTX decrypt_context;

  int end_write (size);
  int pad_bytes (0);

  // initialize the context
  EVP_CIPHER_CTX_init (&decrypt_context);
  EVP_DecryptInit_ex (&decrypt_context, EVP_aes_256_cbc (), NULL,
    key_.get_ptr (), iv_.get_ptr ());
  //EVP_DecryptInit_ex (&decrypt_context, NULL, NULL, NULL, NULL);
  EVPDecryptUpdate (&decrypt_context, source, &end_write,
    source, size);
  EVP_DecryptFinal_ex (&decrypt_context, source + end_write, &pad_bytes);

  result_length = end_write + pad_bytes;

  return result_length;
}

#endif // _USE_SSL_
