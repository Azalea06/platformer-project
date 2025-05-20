#ifndef LEVEL_H
#define LEVEL_H
#include <cstddef>

class level {
    public:

    level(size_t rows, size_t columns, char *data)
        : rows(rows),
          columns(columns),
          data(data) {
    }

    level() : rows{0}, columns{0}, data(nullptr) {};

    [[nodiscard]] size_t get_rows() const {
        return rows;
    }

    [[nodiscard]] size_t get_columns() const {
        return columns;
    }

    [[nodiscard]] char * get_data() const {
        return data;
    }

    void set_rows(const size_t rows) {
        this->rows = rows;
    }

    void set_columns(const size_t columns) {
        this->columns = columns;
    }

    void set_data(char * const data) {
        this->data = data;
    }
private:
    size_t rows = 0, columns = 0;
    char *data = nullptr;
};

#endif //LEVEL_H
