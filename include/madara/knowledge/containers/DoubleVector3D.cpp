#include "DoubleVector3D.h"
#include "madara/knowledge/ContextGuard.h"


madara::knowledge::containers::DoubleVector3D::DoubleVector3D (
  const KnowledgeUpdateSettings & settings,
  const std::string & delimiter)
  : BaseContainer ("", settings), context_ (0), delimiter_ (delimiter)
{
}
   
madara::knowledge::containers::DoubleVector3D::DoubleVector3D (
  const std::string & name,
  KnowledgeBase & knowledge,
  const Dimensions & dimensions,
  bool delete_vars,
  const KnowledgeUpdateSettings & settings,
  const std::string & delimiter)
  : BaseContainer (name, settings), context_ (&(knowledge.get_context ())),
  delimiter_ (delimiter)
{
  size_ = get_size_ref ();
  resize (dimensions, delete_vars);
}
  
madara::knowledge::containers::DoubleVector3D::DoubleVector3D (
  const std::string & name,
  Variables & knowledge,
  const Dimensions & dimensions,
  bool delete_vars,
  const KnowledgeUpdateSettings & settings,
  const std::string & delimiter)
  : BaseContainer (name, settings), context_ (knowledge.get_context ()),
  delimiter_ (delimiter)
{
  size_ = get_size_ref ();
  resize (dimensions, delete_vars);
}

madara::knowledge::containers::DoubleVector3D::DoubleVector3D (
  const DoubleVector3D & rhs)
: BaseContainer (rhs), context_ (rhs.context_),
  vector_ (rhs.vector_),
  size_ (rhs.size_),
  delimiter_ (rhs.delimiter_)
{
}


madara::knowledge::containers::DoubleVector3D::~DoubleVector3D ()
{

}
  
void
madara::knowledge::containers::DoubleVector3D::modify (void)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    Indices dimensions = size ();

    if (dimensions.x > 0 && dimensions.y > 0 && dimensions.z > 0)
    {
      for (size_t i = 0; i < dimensions.x; ++i)
      {
        for (size_t j = 0; j < dimensions.y; ++j)
        {
          for (size_t k = 0; k < dimensions.z; ++k)
          {
            context_->mark_modified (vector_[i][j][k]);
          }
        }
      }
    }

    context_->mark_modified (size_);
  }
}

std::string
madara::knowledge::containers::DoubleVector3D::get_debug_info (void)
{
  std::stringstream result;

  result << "DoubleVector3D: ";

  if (context_)
  {
    ContextGuard context_guard (*context_);

    Indices dimensions = size ();

    result << this->name_;
    result << " [" << dimensions.x << "," << 
                      dimensions.y << "," << dimensions.z << "]";
    result << " = [";

    if (dimensions.x > 0 && dimensions.y > 0)
    {
      for (size_t i = 0; i < dimensions.x; ++i)
      {
        for (size_t j = 0; j < dimensions.y; ++j)
        {
          result << context_->get (vector_[i][j][0]).to_string ();

          for (size_t k = 1; k < dimensions.z; ++k)
          {
            result << ", " << context_->get (vector_[i][j][k]).to_string ();
          }
        }
        result << "\n";
      }
    }

    result << "]";
  }

  return result.str ();
}

void
madara::knowledge::containers::DoubleVector3D::modify_ (void)
{
  modify ();
}

std::string
madara::knowledge::containers::DoubleVector3D::get_debug_info_ (void)
{
  return get_debug_info ();
}

madara::knowledge::containers::BaseContainer *
madara::knowledge::containers::DoubleVector3D::clone (void) const
{
  return new DoubleVector3D (*this);
}

void
madara::knowledge::containers::DoubleVector3D::modify (const Indices & index)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);
    if (index.x < vector_.size () &&
        index.y < vector_[index.x].size () &&
        index.z < vector_[index.x][index.y].size ())
      context_->mark_modified (vector_[index.x][index.y][index.z]);
  }
}

void
madara::knowledge::containers::DoubleVector3D::operator= (
  const DoubleVector3D & rhs)
{
  if (this != &rhs)
  {
    MADARA_GUARD_TYPE guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->size_ = rhs.size_;
    this->vector_ = rhs.vector_;
    this->delimiter_ = rhs.delimiter_;
  }
}

madara::knowledge::VariableReference
madara::knowledge::containers::DoubleVector3D::get_size_ref (void)
{
  VariableReference ref;
  
  if (context_ && name_ != "")
  {
    KnowledgeUpdateSettings keep_local (true);
    std::stringstream buffer;

    ContextGuard context_guard (*context_);

    buffer << name_;
    buffer << delimiter_;
    buffer << "size";

    ref = context_->get_ref (buffer.str (), keep_local);
  }

  return ref;
}

void
madara::knowledge::containers::DoubleVector3D::resize (
  const Dimensions & dimensions, bool delete_vars)
{
  if (context_ && name_ != "")
  {
    ContextGuard context_guard (*context_);

    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "DoubleVector3D::resize: resizing to [%d,%d]\n",
    (int)dimensions.x, (int)dimensions.y);

    bool is_reset = dimensions.x == 0 && dimensions.y == 0;

    if (!size_.is_valid ())
    {
      size_ = get_size_ref ();
    }

    // save the old size
    Dimensions old_size = size ();
    Dimensions new_size (dimensions);

    madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
      "DoubleVector3D::resize: old size is [%d,%d]\n",
    (int)old_size.x, (int)old_size.y);

    if (is_reset)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
        "DoubleVector3D::resize: new size is being reset to size in KB\n");

      new_size.x = old_size.x;
      new_size.y = old_size.y;
      new_size.z = old_size.z;
    }
    else
    {

      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "DoubleVector3D::resize: using dimensions passed in.\n");

      // set the new size
      std::vector <KnowledgeRecord::Integer> update (3);
      update[0] = dimensions.x;
      update[1] = dimensions.y;
      update[2] = dimensions.z;

      context_->set (size_, update, settings_);
    }

    // correct the vector for the new size
    vector_.resize (new_size.x);

    for (size_t i = 0; i < new_size.x; ++i)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
        "DoubleVector3D::resize: resizing vector_[%d] to %d.\n",
        (int)i, (int)new_size.y);

      vector_[i].resize (new_size.y);

      // create any new VariableReference needed, default is end of old cols

      size_t start = old_size.y;


      // if you've gained rows and this is a new row, reset start to 0
      if (is_reset || (old_size.x < new_size.x && i >= old_size.x))
      {
        start = 0;
      }

      madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
        "DoubleVector3D::resize: creating var_refs from %d->%d.\n",
        (int)start, (int)new_size.y);

      // create new VariableReferences
      for (size_t j = start; j < new_size.y; ++j)
      {

        vector_[i][j].resize (new_size.z);

        // create any new VariableReference needed, default is end of old cols

        size_t start_z = old_size.z;

        // if you've gained cols and this is a new col, reset start to 0
        if (is_reset || ((old_size.y < new_size.y && j >= old_size.y) ||
                         (old_size.x < new_size.x && i >= old_size.x)))
        {
          start_z = 0;
        }

        // create new VariableReferences
        for (size_t k = start_z; k < new_size.z; ++k)
        {
          std::stringstream var_name;
          var_name << this->name_;
          var_name << delimiter_;
          var_name << i;
          var_name << delimiter_;
          var_name << j;
          var_name << delimiter_;
          var_name << k;
   
          vector_[i][j][k] = context_->get_ref (var_name.str (), settings_); 
        }
      }
    }

    // delete if we need to delete
    if ((new_size.x < old_size.x || 
         new_size.y < old_size.y || new_size.z < old_size.z) && delete_vars)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "DoubleVector3D::resize: deleting refs: rows: 0->%d.\n",
        (int)old_size.x);

      // delete within the old rows
      for (size_t i = 0; i < old_size.x; ++i)
      {
        // by default, delete from new col size to old col size
        size_t start = new_size.y;

        // the exception is when we are deleting the entire col     
        if (old_size.x > new_size.x && i >= new_size.x)
        {
          start = 0;
        } 

        madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
          "DoubleVector3D::resize: deleting refs: %d:%d->%d.\n",
          (int)i, (int)start, (int)old_size.x);

        // delete old columns
        for (size_t j = start; j < old_size.y; ++j)
        {
          // by default, delete from new col size to old col size
          size_t start_k = new_size.z;

          // the exception is when we are deleting the entire row     
          if (old_size.y > new_size.y && j >= new_size.y)
          {
            start = 0;
          } 

          // delete old columns
          for (size_t k = start_k; k < old_size.z; ++k)
          {
            std::stringstream var_name;
            var_name << this->name_;
            var_name << delimiter_;
            var_name << i;
            var_name << delimiter_;
            var_name << j;
            var_name << delimiter_;
            var_name << k;

            madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
              "DoubleVector3D::resize: deleting ref: %s.\n",
              var_name.str ().c_str ());

            context_->delete_variable (var_name.str (), settings_);
          }
        }
      }
    }
  }
}

madara::knowledge::containers::DoubleVector3D::Indices
madara::knowledge::containers::DoubleVector3D::size (void) const
{
  Indices cur_size;

  if (context_)
  {
    KnowledgeRecord record;
    // lock the KnowledgeBase during access
    {
      ContextGuard context_guard (*context_);

      record = context_->get (size_);
    }
    
    std::vector <KnowledgeRecord::Integer> sizes (record.to_integers ());
    cur_size.x = (size_t) (sizes.size () >= 3 ? sizes[0] : 0);
    cur_size.y = (size_t) (sizes.size () >= 3 ? sizes[1] : 0);
    cur_size.z = (size_t) (sizes.size () >= 3 ? sizes[2] : 0);
  }

  return cur_size;
}

void
madara::knowledge::containers::DoubleVector3D::set_name (
  const std::string & var_name,
  KnowledgeBase & knowledge, const Indices & dimensions)
{
  if (context_ != &(knowledge.get_context ()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context ());

    ContextGuard context_guard (*context_);

    name_ = var_name;

    vector_.clear ();

    size_ = get_size_ref ();
    
    resize (dimensions);
  }
}

void
madara::knowledge::containers::DoubleVector3D::set_name (
  const std::string & var_name,
  Variables & knowledge, const Indices & dimensions)
{
  if (context_ != knowledge.get_context () || name_ != var_name)
  {
    context_ = knowledge.get_context ();

    ContextGuard context_guard (*context_);

    name_ = var_name;

    vector_.clear ();
    resize (dimensions);
  }
}

void
madara::knowledge::containers::DoubleVector3D::set_name (
  const std::string & var_name,
  ThreadSafeContext & knowledge, const Indices & dimensions)
{
  if (context_ != &knowledge || name_ != var_name)
  {
    context_ = &knowledge;

    ContextGuard context_guard (*context_);

    name_ = var_name;

    vector_.clear ();
    resize (dimensions);
  }
}

void
madara::knowledge::containers::DoubleVector3D::set_delimiter (
const std::string & delimiter)
{
  delimiter_ = delimiter;
  if (context_)
  {
    ContextGuard context_guard (*context_);

    vector_.clear ();
    resize ({0,0,0});
  }
}


std::string
madara::knowledge::containers::DoubleVector3D::get_delimiter (void)
{
  return delimiter_;
}

void
madara::knowledge::containers::DoubleVector3D::copy_to (
  std::vector<std::vector<std::vector<type> > > & target) const
{
  KnowledgeUpdateSettings keep_local (true);

  if (context_)
  {
    ContextGuard context_guard (*context_);

    Indices dimensions = size ();

    target.resize (dimensions.x);

    for (size_t i = 0; i < dimensions.x; ++i)
    {
      target[i].resize (dimensions.y);
      for (size_t j = 0; j < dimensions.y; ++j)
      {
        target[i][j].resize (dimensions.z);
        for (size_t k = 0; k < dimensions.z; ++k)
        {
          target[i][j][k] = context_->get (vector_[i][j][k], keep_local).to_double ();
        }
      }
    }
  }
}

madara::knowledge::containers::DoubleVector3D::type
madara::knowledge::containers::DoubleVector3D::operator[] (
  const Indices & index) const
{
  type result (0);

  KnowledgeUpdateSettings keep_local (true);

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "DoubleVector3D::[]: retrieving [%d,%d,%d].\n",
    (int)index.x,(int)index.y,(int)index.z);

  if (context_)
  {
    ContextGuard context_guard (*context_);

    if (index.x < vector_.size () && 
        index.y < vector_[index.x].size () && 
        index.z < vector_[index.x][index.y].size ())
    {
       madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
        "DoubleVector3D::[]: [%d][%d][%d] is within [%d,%d,%d].\n",
        (int)index.x,(int)index.y,(int)index.z,
        (int)vector_.size (),
        (int)vector_[index.x].size (),
        (int)vector_[index.x][index.y].size ());

      result = context_->get (
        vector_[index.x][index.y][index.z], keep_local).to_double ();

       madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
        "DoubleVector3D::[]: value is %.3f.\n",
        result);

    }
    else
    {
       madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
        "DoubleVector3D::[]: [%d][%d][%d] is not within [%d,%d,%d].\n",
        (int)index.x,(int)index.y,(int)index.z,
        (int)vector_.size (),
        (int)vector_[index.x].size (),
        (int)vector_[index.x][index.y].size ());
    }
  }

  return result;
}

bool
madara::knowledge::containers::DoubleVector3D::exists (
  const Indices & index) const
{
  bool result (false);

  if (context_)
  {
    ContextGuard context_guard (*context_);

    if (index.x < vector_.size () && 
        index.y < vector_[index.x].size () && 
        index.z < vector_[index.x][index.y].size ())
    {
      result = context_->exists (vector_[index.x][index.y][index.z]);
    }
  }

  return result;
}

int
madara::knowledge::containers::DoubleVector3D::set (
const Indices & index,
type value)
{
  int result = -1;

  if (context_)
  {
    ContextGuard context_guard (*context_);

    if (index.x < vector_.size () && 
        index.y < vector_[index.x].size () && 
        index.z < vector_[index.x][index.y].size ())
    {
      result = context_->set (
        vector_[index.x][index.y][index.z], value, settings_);
    }
  }

  return result;
}


int
madara::knowledge::containers::DoubleVector3D::set (
const std::vector<std::vector<std::vector<type> > > & value)
{
  int result = 0;

  if (context_)
  {
    ContextGuard context_guard (*context_);

    for (size_t i = 0; i < value.size () && i < vector_.size (); ++i)
    {
      for (size_t j = 0; j < value[i].size () && j < vector_[i].size (); ++j)
      {
        for (size_t k = 0;
               k < value[i][j].size () && k < vector_[i][j].size (); ++k)
        {
          context_->set (vector_[i][j][k], value[i][j][k], settings_);
        }
      }
    }
  }

  return result;
}

int
madara::knowledge::containers::DoubleVector3D::set (
  const Indices & index,
  type value, 
  const KnowledgeUpdateSettings & settings)
{
  int result = -1;
  
  if (context_)
  {
    ContextGuard context_guard (*context_);

    if (index.x < vector_.size () && 
        index.y < vector_[index.x].size () && 
        index.z < vector_[index.x][index.y].size ())
    {
      result = context_->set (vector_[index.x][index.y][index.z], value, settings);
    }
  }

  return result;
}


int
madara::knowledge::containers::DoubleVector3D::set (
  const std::vector<std::vector<std::vector<type> > > & value,
  const KnowledgeUpdateSettings & settings)
{
  int result = 0;

  if (context_)
  {
    ContextGuard context_guard (*context_);

    for (size_t i = 0; i < value.size () && i < vector_.size (); ++i)
    {
      for (size_t j = 0; j < value[i].size () && j < vector_[i].size (); ++j)
      {
        for (size_t k = 0;
               k < value[i][j].size () && k < vector_[i][j].size (); ++k)
        {
          context_->set (vector_[i][j][k], value[i][j][k], settings);
        }
      }
    }
  }

  return result;
}

void
madara::knowledge::containers::DoubleVector3D::set_quality (
  const Indices & index,
  uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  if (context_)
  {
    ContextGuard context_guard (*context_);

    if (index.x < vector_.size () && 
        index.y < vector_[index.x].size () && 
        index.z < vector_[index.x][index.y].size ())
      context_->set_quality (vector_[index.x][index.y][index.z].get_name (),
        quality, true, settings);
  }
}

bool
madara::knowledge::containers::DoubleVector3D::is_true (void) const
{
  bool result (false);

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "DoubleVector3D::is_true: Checking for truth\n");

  if (context_)
  {
    ContextGuard context_guard (*context_);

    result = true;

    madara_logger_log (context_->get_logger (), logger::LOG_MINOR,
      "DoubleVector3D::is_true: context was not null. Result changed to %d\n",
      (int)result);

    for (size_t i = 0; i < vector_.size (); ++i)
    {
      for (size_t j = 0; j < vector_[i].size (); ++i)
      {
        for (size_t k = 0; k < vector_[i][j].size (); ++k)
        {
          madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
            "DoubleVector3D::is_true: checking [%d,%d,%d], is_false of %d. \n",
            (int)i, (int)j, (int)k,
            (int)context_->get (vector_[i][j][k]).is_false ());

          if (context_->get (vector_[i][j][k]).is_false ())
          {
            madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
              "DoubleVector3D::is_true: result is false, breaking\n");

            result = false;
            break;
          }
        }
      }
    }

    if (vector_.size () == 0)
      result = false;
  }

  madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
    "DoubleVector3D::is_true: final result is %d\n", (int)result);

  return result;
}

bool
madara::knowledge::containers::DoubleVector3D::is_false (void) const
{
  return !is_true ();
}


bool
madara::knowledge::containers::DoubleVector3D::is_true_ (void) const
{
  return is_true ();
}

bool
madara::knowledge::containers::DoubleVector3D::is_false_ (void) const
{
  return is_false ();
}

