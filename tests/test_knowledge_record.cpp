#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/Any.h"
#include "test.h"

#include <vector>
#include <memory>
#include <iostream>
#include <limits>

using namespace madara::knowledge;

void check_basic_types_records()
{
  // constuct an integer record and test operators
  KnowledgeRecord int_record = KnowledgeRecord(123);
  TEST_EQ(int_record.type(), KnowledgeRecord::INTEGER);
  TEST_EQ(int_record.is_integer_type(), true);
  TEST_EQ(int_record.is_ref_counted(), false);
  TEST_GT(int_record, KnowledgeRecord(122));
  TEST_NE(int_record, KnowledgeRecord(122));
  TEST_GE(int_record, KnowledgeRecord(123));
  TEST_LE(int_record, KnowledgeRecord(123));
  TEST_LT(int_record, KnowledgeRecord(124));

  // test ++, --, +, - operators
  TEST_EQ(--int_record, 122);
  TEST_EQ(++int_record, 123);
  TEST_EQ(int_record - 10, 113);
  TEST_EQ(int_record + 10, 133);
  TEST_EQ(int_record += 10, 133);
  TEST_NE(int_record += 10, 133);
  TEST_EQ(int_record -= 20, 123);


  // test move and copy constructors, deep_copy
  KnowledgeRecord int_record_copy(int_record);
  TEST_EQ(int_record_copy, 123);
  KnowledgeRecord int_record_move(std::move(int_record_copy));
  TEST_EQ(int_record_move, 123);
  TEST_EQ(int_record_copy, 0);//moved
  int_record_copy = int_record.deep_copy();
  TEST_EQ(int_record_copy, 123);


  //test assign operators
  int_record = KnowledgeRecord(5);
  TEST_EQ(int_record, KnowledgeRecord(5));
  int_record = 6;
  TEST_EQ(int_record, KnowledgeRecord(6));

  // test *, /, % operators
  TEST_EQ(!int_record, false);
  TEST_EQ(int_record * 3, 18);
  TEST_EQ(int_record *= 4, 24);
  TEST_EQ(int_record / 3, 8);
  TEST_EQ(int_record /= 4, 6);
  TEST_EQ(int_record % 4, 2);
  TEST_EQ(int_record %= 5, 1);
  TEST_EQ(int_record, 1);

  //test resize
  int_record.resize(4);
  TEST_EQ(int_record.size(), 4);
  TEST_EQ(int_record.retrieve_index(4), false);

  std::vector<KnowledgeRecord::Integer> int_vec(100, 5);
  // test emplace_integers
  int_record.emplace_integers(int_vec);
  std::shared_ptr<const std::vector<KnowledgeRecord::Integer>>
    int_vec2 = int_record.share_integers();

  TEST_EQ(int_vec2 == nullptr, false);

  //test inc_index, dec_index methods, is_array_type
  int_record.inc_index(0);
  int_record.dec_index(1);
  TEST_EQ(int_record.retrieve_index(0), 6);
  TEST_EQ(int_record.retrieve_index(1), 4);
  TEST_EQ(int_record.is_array_type(), true);

  // test large values
  int_record = KnowledgeRecord(std::numeric_limits<int64_t>::max());
  TEST_EQ(int_record, std::numeric_limits<int64_t>::max());
  int_record += 1;
  // should be overflow
  TEST_EQ(int_record, std::numeric_limits<int64_t>::min());


  TEST_LT(int_record, KnowledgeRecord(10));
  TEST_LT(int_record, KnowledgeRecord(10.1));


  KnowledgeRecord int_array_record = KnowledgeRecord(tags::integers, 100, 10);

  int_array_record.emplace_doubles(std::vector<double>(20, 5.5));
  TEST_EQ(int_array_record.share_doubles() == nullptr, false);

  //check set_index method, get_precision, set_precision
  int_array_record.set_index(10, 5.55);
  int_array_record.set_precision(2);
  TEST_EQ(int_array_record.get_precision(), 2);
  int_array_record.set_index(11, 100);
  TEST_EQ(int_array_record.retrieve_index(10), 5.55);
  TEST_EQ(int_array_record.retrieve_index(11), 100);

  int_array_record.emplace_integers(std::vector<KnowledgeRecord::Integer>());
  TEST_EQ(int_array_record.size(), 0);
  int_array_record.inc_index(4);
  TEST_EQ(int_array_record.size(), 5);
  std::vector<KnowledgeRecord::Integer> int_vector(5);
  ++int_vector[4];
  TEST_EQ(int_array_record, KnowledgeRecord(int_vector));


  //test large array
  int_vector.resize(1000000, 10);
  int_array_record.emplace_integers(std::move(int_vector));
  TEST_EQ(int_vector.size(), 0);
  TEST_EQ(int_array_record.size(), 1000000);

  KnowledgeRecord string_record = KnowledgeRecord("test string");
  TEST_EQ(string_record.type(), KnowledgeRecord::STRING);
  // test operator= and operator==
  TEST_EQ(string_record == "test string", true);
  string_record = "another string";
  TEST_EQ(string_record, KnowledgeRecord("another string"));

  // check is_false for string, is_ref_counted
  TEST_EQ(string_record.is_true(), true);
  TEST_EQ((bool)string_record, true);
  string_record = "";
  TEST_EQ(string_record.is_false(), true);
  TEST_EQ(string_record.is_ref_counted(), true);

  std::string empty_string;
  TEST_EQ(string_record, empty_string);
  string_record.emplace_string(empty_string);

  //test empty string, long string and retrive_index with no array
  TEST_EQ(string_record, empty_string);
  string_record.emplace_string("some more test string with not very shord size to be tested in knowledge record");
  TEST_EQ(string_record.to_string(), "some more test string with not very shord size to be tested in knowledge record");
  TEST_EQ(string_record.retrieve_index(10), false);

  // test assigning integer
  string_record = 0x12;
  TEST_EQ(string_record.type(), KnowledgeRecord::INTEGER);
  TEST_EQ(string_record, 18);

  // test inc converts to array
  TEST_EQ(string_record.inc_index(0), 1);
  TEST_EQ(string_record.type(), KnowledgeRecord::INTEGER_ARRAY);

  // test apply
  ThreadSafeContext context;
  int result = string_record.apply(context, "apply_check", 10, 100, false);
  TEST_EQ(result, 1);

  //test reset_value
  string_record.reset_value();
  TEST_EQ(string_record.type(), KnowledgeRecord::EMPTY);

}

void check_non_basic_types_records()
{
  KnowledgeRecord record(2);
  std::vector<unsigned char> data = {0, 1, 2, 4};
  record.emplace_any(data);
  TEST_EQ(record.type(), KnowledgeRecord::ANY);
  // it throws error if no any type
  ConstAnyRef ref = record.get_any_cref();
  TEST_EQ(record.is_any_type(), true);
  TEST_EQ(ref.to_string().find("ANY") != std::string::npos, true);

  //check clone
  KnowledgeRecord* cloned_rec = record.clone();
  TEST_EQ(cloned_rec == nullptr, false);


  // check write and getting encoded size
  cloned_rec->emplace_string("writing string");
  int64_t buf_size = cloned_rec->get_encoded_size();
  char* buf = new char[buf_size];
  TEST_GT(buf_size, 0);
  cloned_rec->write(buf, buf_size);
  TEST_EQ(buf == nullptr, false);

  // make sure share any doesn't share anything and share_string works
  TEST_EQ(cloned_rec->is_any_type(), false);
  TEST_EQ(cloned_rec->share_any() == nullptr, true);
  TEST_EQ(cloned_rec->share_string() == nullptr, false);

  // make sure copied into any
  Any output_any = cloned_rec->to_any();
  TEST_EQ(output_any.ref<std::string>(), "writing string");

  TEST_EQ(buf_size, 0);
  buf_size = cloned_rec->get_encoded_size();

  cloned_rec->emplace_any(std::vector<double>(100));
  TEST_EQ(cloned_rec->get_any_ref().to_doubles().size(), 100);

  cloned_rec->emplace_any(Any(nullptr));
  TEST_EQ(cloned_rec->type(), KnowledgeRecord::ANY);
  TEST_EQ(cloned_rec->share_any() == nullptr, false);


  delete cloned_rec;


  //check read method and size
  KnowledgeRecord new_rec(0);
  new_rec.read(buf, buf_size);

  TEST_EQ(new_rec.size(), 15);
  TEST_EQ(new_rec.is_string_type(), true);

  delete buf;

}

void check_file_records()
{
  //create a record and read a file into it
  KnowledgeRecord image_rec;
  std::string image = madara::utility::expand_envs ("$(MADARA_ROOT)/tests/images/manaus_hotel_100x100.jpg");
  image_rec.read_file(image);

  TEST_EQ(image_rec.type(), KnowledgeRecord::IMAGE_JPEG);

  std::string other_image = madara::utility::expand_envs ("$(MADARA_ROOT)/tests/images/manaus_hotel_100x167.jpg");

  KnowledgeRecord image_rec2;
  image_rec2.read_file(other_image);

  unsigned char* buffer = nullptr;
  size_t size;
  // to_unmanaged_buffer check
  buffer = image_rec2.to_unmanaged_buffer(size);

  TEST_GT(size, 0);
  TEST_EQ(buffer == nullptr, false);

  KnowledgeRecord other_file;
  std::string cpp = madara::utility::expand_envs ("$(MADARA_ROOT)/tests/test_knowledge_record.cpp");
  other_file.read_file(cpp);
  TEST_EQ(other_file.is_file_type(), true);

  other_file.set_jpeg(buffer, size);
  TEST_EQ(other_file.type(), KnowledgeRecord::IMAGE_JPEG);
  TEST_GT(other_file.size(), 0);

  unsigned char* buffer_new = (unsigned char* )"123456789";
  other_file.set_file(buffer_new, 4);
  size_t size_new;
  unsigned char* buffer2 = other_file.to_unmanaged_buffer(size_new);

  TEST_EQ(size_new, 4);
  char* check_buf = "1234";
  TEST_EQ(std::string((char*)buffer2, 4), std::string(check_buf, 4));

  other_file.set_file(buffer, 0);
  TEST_EQ(other_file.size(), 0);// shall not cause crash until size is 0

  // emplace file from buffer of image
  std::vector<unsigned char> buffer_vec(buffer, buffer + size);
  other_file.emplace_file(std::move(buffer_vec));
  TEST_EQ(other_file.is_file_type(), true);
  TEST_EQ(other_file.size(), size);
  // shall be the same bit
  TEST_EQ(other_file.to_unmanaged_buffer(size)[100], buffer[100]);

  delete buffer2;
  buffer2 = nullptr;

  delete buffer;
  buffer = nullptr;

}

int main ()
{
  check_basic_types_records();

  check_non_basic_types_records();

  check_file_records();


  // this test is checking knowledge record functionaly
  // so you get FAILs this means KnowledgeRecord class is not working properly

  return 0;
}
