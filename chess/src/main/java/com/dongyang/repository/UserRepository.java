package com.dongyang.repository;

import com.dongyang.domain.User;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.transaction.annotation.Transactional;

/**
 * Created by HeemangHan on 2016. 10. 25..
 */
public interface UserRepository extends JpaRepository<User, Integer> {
    User findByIdAndPassword(String id, String password);

    @Modifying
    @Transactional
    @Query("update User set password = :password")
    int updateByPassword(@Param("password") String password);
}
