package com.dongyang.config;

import com.dongyang.filter.CORSFilter;
import org.springframework.boot.web.servlet.FilterRegistrationBean;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

/**
 * Created by HeemangHan on 2016. 10. 25..
 */

@Configuration
public class FilterConfig {
    @Bean
    public FilterRegistrationBean getFilterRegistrationBean() {
        FilterRegistrationBean registrationBean = new FilterRegistrationBean(new CORSFilter());
        return registrationBean;
    }
}
