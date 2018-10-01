#include "DoubleVector2D.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::DoubleVector2D::DoubleVector2D(
    const KnowledgeUpdateSettings& settings, const std::string& delimiter)
  : BaseContainer("", settings), context_(0), delimiter_(delimiter)
{
}

madara::knowledge::containers::DoubleVector2D::DoubleVector2D(
    const std::string& name, KnowledgeBase& knowledge,
    const Dimensions& dimensions, bool delete_vars,
    const KnowledgeUpdateSettings& settings, const std::string& delimiter)
  : BaseContainer(name, settings),
    context_(&(knowledge.get_context())),
    delimiter_(delimiter)
{
  size_ = get_size_ref();
  resize(dimensions, delete_vars);
}

madara::knowledge::containers::DoubleVector2D::DoubleVector2D(
    const std::string& name, Variables& knowledge, const Dimensions& dimensions,
    bool delete_vars, const KnowledgeUpdateSettings& settings,
    const std::string& delimiter)
  : BaseContainer(name, settings),
    context_(knowledge.get_context()),
    delimiter_(delimiter)
{
  size_ = get_size_ref();
  resize(dimensions, delete_vars);
}

madara::knowledge::containers::DoubleVector2D::DoubleVector2D(
    const DoubleVector2D& rhs)
  : BaseContainer(rhs),
    context_(rhs.context_),
    vector_(rhs.vector_),
    size_(rhs.size_),
    delimiter_(rhs.delimiter_)
{
}

madara::knowledge::containers::DoubleVector2D::~DoubleVector2D() {}

void madara::knowledge::containers::DoubleVector2D::modify(void)
{
  if (context_ && name_ != "") {
    ContextGuard context_guard(*context_);

    Indices dimensions = size();

    if (dimensions.x > 0 && dimensions.y > 0) {
      for (size_t i = 0; i < dimensions.x; ++i) {
        for (size_t j = 0; j < dimensions.y; ++j) {
          context_->mark_modified(vector_[i][j]);
        }
      }
    }

    context_->mark_modified(size_);
  }
}

std::string madara::knowledge::containers::DoubleVector2D::get_debug_info(void)
{
  std::stringstream result;

  result << "DoubleVector2D: ";

  if (context_) {
    ContextGuard context_guard(*context_);

    Indices dimensions = size();

    result << this->name_;
    result << " [" << dimensions.x << "," << dimensions.y << "]";
    result << " = [";

    if (dimensions.x > 0 && dimensions.y > 0) {
      for (size_t i = 0; i < dimensions.x; ++i) {
        result << context_->get(vector_[i][0]).to_string();

        for (size_t j = 1; j < dimensions.y; ++j) {
          result << ", " << context_->get(vector_[i][j]).to_string();
        }
        result << "\n";
      }
    }

    result << "]";
  }

  return result.str();
}

void madara::knowledge::containers::DoubleVector2D::modify_(void)
{
  modify();
}

std::string madara::knowledge::containers::DoubleVector2D::get_debug_info_(void)
{
  return get_debug_info();
}

madara::knowledge::containers::BaseContainer*
madara::knowledge::containers::DoubleVector2D::clone(void) const
{
  return new DoubleVector2D(*this);
}

void madara::knowledge::containers::DoubleVector2D::modify(const Indices& index)
{
  if (context_) {
    ContextGuard context_guard(*context_);
    if (index.x < vector_.size() && index.y < vector_[index.x].size())
      context_->mark_modified(vector_[index.x][index.y]);
  }
}

void madara::knowledge::containers::DoubleVector2D::operator=(
    const DoubleVector2D& rhs)
{
  if (this != &rhs) {
    MADARA_GUARD_TYPE guard(mutex_), guard2(rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->size_ = rhs.size_;
    this->vector_ = rhs.vector_;
    this->delimiter_ = rhs.delimiter_;
  }
}

madara::knowledge::VariableReference
madara::knowledge::containers::DoubleVector2D::get_size_ref(void)
{
  VariableReference ref;

  if (context_ && name_ != "") {
    KnowledgeUpdateSettings keep_local(true);
    std::stringstream buffer;

    ContextGuard context_guard(*context_);

    buffer << name_;
    buffer << delimiter_;
    buffer << "size";

    ref = context_->get_ref(buffer.str(), keep_local);
  }

  return ref;
}

void madara::knowledge::containers::DoubleVector2D::resize(
    const Dimensions& dimensions, bool delete_vars)
{
  if (context_ && name_ != "") {
    ContextGuard context_guard(*context_);

    madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
        "DoubleVector2D::resize: resizing to [%d,%d]\n", (int)dimensions.x,
        (int)dimensions.y);

    bool is_reset = dimensions.x == 0 && dimensions.y == 0;

    if (!size_.is_valid()) {
      size_ = get_size_ref();
    }

    // save the old size
    Dimensions old_size = size();
    Dimensions new_size(dimensions);

    madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
        "DoubleVector2D::resize: old size is [%d,%d]\n", (int)old_size.x,
        (int)old_size.y);

    if (is_reset) {
      madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
          "DoubleVector2D::resize: new size is being reset to size in KB\n");

      new_size.x = old_size.x;
      new_size.y = old_size.y;
    } else {
      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "DoubleVector2D::resize: using dimensions passed in.\n");

      // set the new size
      std::vector<KnowledgeRecord::Integer> update(2);
      update[0] = dimensions.x;
      update[1] = dimensions.y;

      context_->set(size_, update, settings_);
    }

    // correct the vector for the new size
    vector_.resize(new_size.x);

    for (size_t i = 0; i < new_size.x; ++i) {
      madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
          "DoubleVector2D::resize: resizing vector_[%d] to %d.\n", (int)i,
          (int)new_size.y);

      vector_[i].resize(new_size.y);

      // create any new VariableReference needed, default is end of old cols

      size_t start = old_size.y;

      // if you've gained rows and this is a new row, reset start to 0
      if (is_reset || (old_size.x < new_size.x && i >= old_size.x)) {
        start = 0;
      }

      madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
          "DoubleVector2D::resize: creating var_refs from %d->%d.\n",
          (int)start, (int)new_size.y);

      // create new VariableReferences
      for (size_t j = start; j < new_size.y; ++j) {
        std::stringstream var_name;
        var_name << this->name_;
        var_name << delimiter_;
        var_name << i;
        var_name << delimiter_;
        var_name << j;

        vector_[i][j] = context_->get_ref(var_name.str(), settings_);
      }
    }

    // delete if we need to delete
    if ((new_size.x < old_size.x || new_size.y < old_size.y) && delete_vars) {
      madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
          "DoubleVector2D::resize: deleting refs: rows: 0->%d.\n",
          (int)old_size.x);

      // delete within the old rows
      for (size_t i = 0; i < old_size.x; ++i) {
        // by default, delete from new col size to old col size
        size_t start = new_size.y;

        // the exception is when we are deleting the entire row
        if (old_size.x > new_size.x && i >= new_size.x) {
          start = 0;
        }

        madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
            "DoubleVector2D::resize: deleting refs: %d:%d->%d.\n", (int)i,
            (int)start, (int)old_size.x);

        // delete old columns
        for (size_t j = start; j < old_size.y; ++j) {
          std::stringstream var_name;
          var_name << this->name_;
          var_name << delimiter_;
          var_name << i;
          var_name << delimiter_;
          var_name << j;

          madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
              "DoubleVector2D::resize: deleting ref: %s.\n",
              var_name.str().c_str());

          context_->delete_variable(var_name.str(), settings_);
        }
      }
    }
  }
}

madara::knowledge::containers::DoubleVector2D::Indices
madara::knowledge::containers::DoubleVector2D::size(void) const
{
  Indices cur_size;

  if (context_) {
    KnowledgeRecord record;
    // lock the KnowledgeBase during access
    {
      ContextGuard context_guard(*context_);

      record = context_->get(size_);
    }

    std::vector<KnowledgeRecord::Integer> sizes(record.to_integers());
    cur_size.x = (size_t)(sizes.size() >= 2 ? sizes[0] : 0);
    cur_size.y = (size_t)(sizes.size() >= 2 ? sizes[1] : 0);
  }

  return cur_size;
}

void madara::knowledge::containers::DoubleVector2D::set_name(
    const std::string& var_name, KnowledgeBase& knowledge,
    const Indices& dimensions)
{
  if (context_ != &(knowledge.get_context()) || name_ != var_name) {
    context_ = &(knowledge.get_context());

    ContextGuard context_guard(*context_);

    name_ = var_name;

    vector_.clear();

    size_ = get_size_ref();

    resize(dimensions);
  }
}

void madara::knowledge::containers::DoubleVector2D::set_name(
    const std::string& var_name, Variables& knowledge,
    const Indices& dimensions)
{
  if (context_ != knowledge.get_context() || name_ != var_name) {
    context_ = knowledge.get_context();

    ContextGuard context_guard(*context_);

    name_ = var_name;

    vector_.clear();
    resize(dimensions);
  }
}

void madara::knowledge::containers::DoubleVector2D::set_name(
    const std::string& var_name, ThreadSafeContext& knowledge,
    const Indices& dimensions)
{
  if (context_ != &knowledge || name_ != var_name) {
    context_ = &knowledge;

    ContextGuard context_guard(*context_);

    name_ = var_name;

    vector_.clear();
    resize(dimensions);
  }
}

void madara::knowledge::containers::DoubleVector2D::set_delimiter(
    const std::string& delimiter)
{
  delimiter_ = delimiter;
  if (context_) {
    ContextGuard context_guard(*context_);

    vector_.clear();
    resize({0, 0});
  }
}

std::string madara::knowledge::containers::DoubleVector2D::get_delimiter(void)
{
  return delimiter_;
}

void madara::knowledge::containers::DoubleVector2D::copy_to(
    std::vector<std::vector<type> >& target) const
{
  KnowledgeUpdateSettings keep_local(true);

  if (context_) {
    ContextGuard context_guard(*context_);

    Indices dimensions = size();

    target.resize(dimensions.x);

    for (size_t i = 0; i < dimensions.x; ++i) {
      target[i].resize(dimensions.y);
      for (size_t j = 0; j < dimensions.y; ++j) {
        target[i][j] = context_->get(vector_[i][j], keep_local).to_double();
      }
    }
  }
}

madara::knowledge::containers::DoubleVector2D::type
    madara::knowledge::containers::DoubleVector2D::operator[](
        const Indices& index) const
{
  type result(0);

  KnowledgeUpdateSettings keep_local(true);

  if (context_) {
    ContextGuard context_guard(*context_);

    if (index.x < vector_.size() && index.y < vector_[index.x].size()) {
      result = context_->get(vector_[index.x][index.y], keep_local).to_double();
    }
  }

  return result;
}

bool madara::knowledge::containers::DoubleVector2D::exists(
    const Indices& index) const
{
  bool result(false);

  if (context_) {
    ContextGuard context_guard(*context_);

    if (index.x < vector_.size() && index.y < vector_[index.x].size()) {
      result = context_->exists(vector_[index.x][index.y]);
    }
  }

  return result;
}

int madara::knowledge::containers::DoubleVector2D::set(
    const Indices& index, type value)
{
  int result = -1;

  if (context_) {
    ContextGuard context_guard(*context_);

    if (index.x < vector_.size() && index.y < vector_[index.x].size()) {
      result = context_->set(vector_[index.x][index.y], value, settings_);
    }
  }

  return result;
}

int madara::knowledge::containers::DoubleVector2D::set(
    const std::vector<std::vector<type> >& value)
{
  int result = 0;

  if (context_) {
    ContextGuard context_guard(*context_);

    for (size_t i = 0; i < value.size() && i < vector_.size(); ++i) {
      for (size_t j = 0; j < value[i].size() && j < vector_[i].size(); ++j) {
        context_->set(vector_[i][j], value[i][j], settings_);
      }
    }
  }

  return result;
}

int madara::knowledge::containers::DoubleVector2D::set(
    const Indices& index, type value, const KnowledgeUpdateSettings& settings)
{
  int result = -1;

  if (context_) {
    ContextGuard context_guard(*context_);

    if (index.x < vector_.size() && index.y < vector_[index.x].size()) {
      result = context_->set(vector_[index.x][index.y], value, settings);
    }
  }

  return result;
}

int madara::knowledge::containers::DoubleVector2D::set(
    const std::vector<std::vector<type> >& value,
    const KnowledgeUpdateSettings& settings)
{
  int result = 0;

  if (context_) {
    ContextGuard context_guard(*context_);

    for (size_t i = 0; i < value.size() && i < vector_.size(); ++i) {
      for (size_t j = 0; j < value[i].size() && j < vector_[i].size(); ++j) {
        context_->set(vector_[i][j], value[i][j], settings);
      }
    }
  }

  return result;
}

void madara::knowledge::containers::DoubleVector2D::set_quality(
    const Indices& index, uint32_t quality,
    const KnowledgeReferenceSettings& settings)
{
  if (context_) {
    ContextGuard context_guard(*context_);

    if (index.x < vector_.size() && index.y < vector_[index.x].size())
      context_->set_quality(
          vector_[index.x][index.y].get_name(), quality, true, settings);
  }
}

bool madara::knowledge::containers::DoubleVector2D::is_true(void) const
{
  bool result(false);

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "DoubleVector2D::is_true: Checking for truth\n");

  if (context_) {
    ContextGuard context_guard(*context_);

    result = true;

    madara_logger_log(context_->get_logger(), logger::LOG_MINOR,
        "DoubleVector2D::is_true: context was not null. Result changed to %d\n",
        (int)result);

    for (size_t i = 0; i < vector_.size(); ++i) {
      for (size_t j = 0; j < vector_[i].size(); ++i) {
        madara_logger_log(context_->get_logger(), logger::LOG_DETAILED,
            "DoubleVector2D::is_true: checking [%d,%d], is_false of %d. \n",
            (int)i, (int)j, (int)context_->get(vector_[i][j]).is_false());

        if (context_->get(vector_[i][j]).is_false()) {
          madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
              "DoubleVector2D::is_true: result is false, breaking\n");

          result = false;
          break;
        }
      }
    }

    if (vector_.size() == 0)
      result = false;
  }

  madara_logger_log(context_->get_logger(), logger::LOG_MAJOR,
      "DoubleVector2D::is_true: final result is %d\n", (int)result);

  return result;
}

bool madara::knowledge::containers::DoubleVector2D::is_false(void) const
{
  return !is_true();
}

bool madara::knowledge::containers::DoubleVector2D::is_true_(void) const
{
  return is_true();
}

bool madara::knowledge::containers::DoubleVector2D::is_false_(void) const
{
  return is_false();
}
