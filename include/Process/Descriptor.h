#ifndef INCLUDE_PROCESS_DESCRIPTOR_H_
#define INCLUDE_PROCESS_DESCRIPTOR_H_

namespace process {
class Descriptor {
 public:
    Descriptor();
    explicit Descriptor(int fd);
    ~Descriptor() noexcept;

    Descriptor(Descriptor&&) noexcept;
    Descriptor& operator=(Descriptor&& new_des) noexcept;

    Descriptor& operator=(int fd);

    void close();
    int get() const;
    int extract();
    bool operator<(int num);

    Descriptor(Descriptor&) = delete;
    Descriptor& operator=(Descriptor&) = delete;
 private:
    int _fd = -1;
};
}  // namespace process

#endif  // INCLUDE_PROCESS_DESCRIPTOR_H_
