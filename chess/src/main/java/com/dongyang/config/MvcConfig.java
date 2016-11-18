package com.dongyang.config;

import com.dongyang.filter.Interceptor;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Configuration;
import org.springframework.web.servlet.config.annotation.InterceptorRegistry;
import org.springframework.web.servlet.config.annotation.ViewControllerRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurerAdapter;

/**
 * Created by HeemangHan on 2016. 10. 25..
 */

@Configuration
public class MvcConfig extends WebMvcConfigurerAdapter {

    @Autowired
    private Interceptor interceptor;

    @Override
    public void addViewControllers(ViewControllerRegistry registry) {
        registry.addViewController("/").setViewName("main");
        registry.addViewController("/main").setViewName("main");
        registry.addViewController("/login").setViewName("login");
    }

    @Override
    public void addInterceptors(InterceptorRegistry registry) {
        registry.addInterceptor(interceptor)
                .addPathPatterns("/")
                .addPathPatterns("/main");
    }
}
