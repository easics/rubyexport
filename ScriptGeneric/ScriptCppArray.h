// This file is part of rubyexport.
//
// rubyexport is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// rubyexport is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// rubyexport. If not, see <https://www.gnu.org/licenses/>.

#ifndef ScriptCppArray_h_
#define ScriptCppArray_h_

#ifdef SCRIPT_PYTHON
struct PythonScriptCppArrayInstance;
#endif

class ScriptCppArrayBase
{
public:
  ScriptCppArrayBase(void * language);

  static void init();

  virtual void clear() = 0;
#ifdef SCRIPT_RUBY
  virtual void ruby_push(VALUE value) = 0;
  virtual void ruby_setelement(int index, VALUE value) = 0;
  virtual void ruby_delete(VALUE value) = 0;
  virtual void ruby_delete_at(int index) = 0;
  virtual void ruby_pop() = 0;
  virtual void ruby_reverse() = 0;
  virtual void ruby_rotate(int count) = 0;
  virtual void ruby_shift() = 0;
  virtual int ruby_size() = 0;
  virtual void ruby_unshift(VALUE arg) = 0;
  VALUE rubyHandle_;
#endif
#ifdef SCRIPT_PYTHON
  virtual void python_setelement(long index, PyObject * value) = 0;
  virtual void python_append(PyObject * value) = 0;
  virtual void python_insert(long index, PyObject * value) = 0;
  virtual void python_pop(long index) = 0;
  virtual void python_remove(PyObject * item) = 0;
  virtual void python_reverse() = 0;
  PythonScriptCppArrayInstance * pythonHandle_;
#endif
  virtual std::string info() const = 0;
  virtual unsigned int length() const = 0;
protected:
#ifdef SCRIPT_RUBY
  static VALUE rubyClassType_;
#endif
};

template <typename T>
class ScriptCppArray : public ScriptCppArrayBase
{
public:
  ScriptCppArray(std::vector<T> * cppArray, void * language);

  virtual void clear() override;
#ifdef SCRIPT_RUBY
  virtual void ruby_push(VALUE value) override;
  virtual void ruby_setelement(int index, VALUE value) override;
  virtual void ruby_delete(VALUE value) override;
  virtual void ruby_delete_at(int index) override;
  virtual void ruby_pop() override;
  virtual void ruby_reverse() override;
  virtual void ruby_rotate(int count) override;
  virtual void ruby_shift() override;
  virtual int ruby_size() override;
  virtual void ruby_unshift(VALUE arg) override;
#endif
#ifdef SCRIPT_PYTHON
  virtual void python_setelement(long index, PyObject * value) override;
  virtual void python_append(PyObject * value) override;
  virtual void python_insert(long index, PyObject * value) override;
  virtual void python_pop(long index) override;
  virtual void python_reverse() override;
  virtual void python_remove(PyObject * item) override;
#endif
  std::string info() const override;
  virtual unsigned int length() const override;
private:
  std::vector<T> * cppArray_;
};

// cppArray can be nullptr (for temporary objects)
template<typename T>
ScriptCppArray<T>::ScriptCppArray(std::vector<T> * cppArray, void * language)
  : ScriptCppArrayBase(language), cppArray_(cppArray)
{
}

template<typename T>
void ScriptCppArray<T>::clear()
{
  if (cppArray_)
    cppArray_->clear();
}

#ifdef SCRIPT_RUBY
template<typename T>
void ScriptCppArray<T>::ruby_push(VALUE value)
{
  if (cppArray_)
    {
      T element;
      ReflectionHandle rubyElement;
      rubyElement.rubyHandle = value;
      ReflectionWrite(rubyElement, element, LANGUAGE_RUBY);
      cppArray_->push_back(element);
    }
}

template<typename T>
void ScriptCppArray<T>::ruby_setelement(int index, VALUE value)
{
  if (cppArray_)
    {
      T element;
      ReflectionHandle rubyElement;
      rubyElement.rubyHandle = value;
      ReflectionWrite(rubyElement, element, LANGUAGE_RUBY);
      if (index < 0)
        {
          (*cppArray_)[cppArray_->size() + index] = element;
        }
      else
        {
          (*cppArray_)[index] = element;
        }
    }
}

template<typename T>
void ScriptCppArray<T>::ruby_delete(VALUE value)
{
  if (cppArray_)
    {
      T element;
      ReflectionHandle rubyElement;
      rubyElement.rubyHandle = value;
      ReflectionWrite(rubyElement, element, LANGUAGE_RUBY);
      for (auto cElement = cppArray_->begin(); cElement != cppArray_->end();)
        {
          if (*cElement == element)
            cElement = cppArray_->erase(cElement);
          else
            ++cElement;
        }
    }
}

template<typename T>
void ScriptCppArray<T>::ruby_delete_at(int index)
{
  if (cppArray_)
    {
      if (index < 0)
        index = cppArray_->size() + index;
      cppArray_->erase(cppArray_->begin() + index);
    }
}

template<typename T>
void ScriptCppArray<T>::ruby_pop()
{
  if (cppArray_)
    {
      cppArray_->pop_back();
    }
}

template<typename T>
void ScriptCppArray<T>::ruby_reverse()
{
  if (cppArray_)
    {
      std::reverse(cppArray_->begin(), cppArray_->end());
    }
}

template<typename T>
void ScriptCppArray<T>::ruby_rotate(int count)
{
  if (cppArray_)
    {
      if (count > 0)
        {
          count = count % cppArray_->size();
        }
      else
        {
          count = cppArray_->size() - (-count % cppArray_->size());
        }
      std::rotate(cppArray_->begin(), cppArray_->begin() + count,
                  cppArray_->end());
    }
}

template<typename T>
void ScriptCppArray<T>::ruby_shift()
{
  if (cppArray_)
    {
      cppArray_->erase(cppArray_->begin());
    }
}

template<typename T>
int ScriptCppArray<T>::ruby_size()
{
  if (cppArray_)
    {
      return cppArray_->size();
    }
  return 0;
}

template<typename T>
void ScriptCppArray<T>::ruby_unshift(VALUE arg)
{
  if (cppArray_)
    {
      T element;
      ReflectionHandle rubyElement;
      rubyElement.rubyHandle = arg;
      ReflectionWrite(rubyElement, element, LANGUAGE_RUBY);
      cppArray_->insert(cppArray_->begin(), element);
    }
}

#endif

#ifdef SCRIPT_PYTHON
template<typename T>
void ScriptCppArray<T>::python_setelement(long index, PyObject * value)
{
  if (cppArray_)
    {
      T element;
      ReflectionHandle pythonElement;
      pythonElement.pythonHandle = value;
      ReflectionWrite(pythonElement, element, LANGUAGE_PYTHON);
      if (index < 0)
        {
          (*cppArray_)[cppArray_->size() + index] = element;
        }
      else
        {
          (*cppArray_)[index] = element;
        }
    }
}

template<typename T>
void ScriptCppArray<T>::python_append(PyObject * value)
{
  if (cppArray_)
    {
      T element;
      ReflectionHandle pythonElement;
      pythonElement.pythonHandle = value;
      ReflectionWrite(pythonElement, element, LANGUAGE_PYTHON);
      cppArray_->push_back(element);
    }
}

template<typename T>
void ScriptCppArray<T>::python_insert(long index, PyObject * value)
{
  if (cppArray_)
    {
      T element;
      ReflectionHandle pythonElement;
      pythonElement.pythonHandle = value;
      ReflectionWrite(pythonElement, element, LANGUAGE_PYTHON);
      cppArray_->insert(cppArray_->begin()+index, element);
    }
}

template<typename T>
void ScriptCppArray<T>::python_pop(long index)
{
  if (cppArray_)
    {
      if (index == -1)
        cppArray_->pop_back();
      else
        cppArray_->erase(cppArray_->begin() + index);
    }
}

template<typename T>
void ScriptCppArray<T>::python_remove(PyObject * item)
{
  if (cppArray_)
    {
      T element;
      ReflectionHandle pythonElement;
      pythonElement.pythonHandle = item;
      ReflectionWrite(pythonElement, element, LANGUAGE_PYTHON);
      for (auto cElement = cppArray_->begin(); cElement != cppArray_->end();)
        {
          if (*cElement == element)
            {
              cppArray_->erase(cElement);
              break;
            }
          else
            ++cElement;
        }
    }
}

template<typename T>
void ScriptCppArray<T>::python_reverse()
{
  if (cppArray_)
    {
      std::reverse(cppArray_->begin(), cppArray_->end());
    }
}

#endif

template<typename T>
std::string ScriptCppArray<T>::info() const
{
  char buf[16];
  sprintf(buf, "%p %p %lu", this, cppArray_, cppArray_->size());
  return buf;
}

template<typename T>
unsigned int ScriptCppArray<T>::length() const
{
  return cppArray_->size();
}

#endif
